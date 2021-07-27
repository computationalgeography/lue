#include "lue/framework/algorithm/accu_threshold.hpp"
#include "lue/framework/algorithm/copy.hpp"
#include "lue/framework/algorithm/default_policies/uniform.hpp"
#include "lue/framework/benchmark/benchmark_model.hpp"
#include "lue/framework/benchmark/hpx_main.hpp"
#include "lue/framework/benchmark/model_benchmark.hpp"
#include "lue/framework/io/read_into.hpp"
#include "lue/data_model/hl.hpp"
#include "lue/docopt.hpp"


namespace lue::benchmark {

    template<
        typename Count,
        Rank rank>
    hdf5::Shape shape_to_shape(
        lue::Shape<Count, rank> const& from_shape)
    {
        hdf5::Shape to_shape(rank);

        for(std::size_t i = 0; i < rank; ++i)
        {
            to_shape[i] = from_shape[i];
        }

        return to_shape;
    }


    template<
        typename MaterialElement>
    class AccuThresholdBenchmarkModel final:
        public BenchmarkModel<MaterialElement, 2>
    {

        private:

               using FlowDirectionElement = std::uint8_t;

        public:

            AccuThresholdBenchmarkModel(
                Task const& task,
                std::size_t max_tree_depth,
                std::string const& array_pathname,
                std::vector<Index> center_cell):

                BenchmarkModel<MaterialElement, 2>{task, max_tree_depth},
                _array_pathname{array_pathname},
                _hyperslab{},
                _object_id{}

            {
                using Dataset = data_model::Dataset;
                using RasterView = data_model::constant::RasterView<Dataset*>;

                auto const [dataset_pathname, phenomenon_name, property_set_name, layer_name] =
                    parse_array_pathname(array_pathname);

                Dataset dataset{dataset_pathname};
                RasterView raster_view{&dataset, phenomenon_name, property_set_name};

                if(center_cell.empty())
                {
                    // Default center cell is the center cell of the array
                    hdf5::Shape const& grid_shape{raster_view.grid_shape()};

                    center_cell.push_back(grid_shape[0] / 2);
                    center_cell.push_back(grid_shape[1] / 2);
                }

                lue_hpx_assert(center_cell.size() == 2);

                hdf5::Shape const array_shape{shape_to_shape(this->array_shape())};

                hdf5::Offset start{
                        center_cell[0] - (array_shape[0] / 2),
                        center_cell[1] - (array_shape[1] / 2)
                    };
                hdf5::Count count{array_shape[0], array_shape[1]};

                _hyperslab = hdf5::Hyperslab{start, count};
                _object_id = raster_view.object_id();

                lue_hpx_assert(_hyperslab.nr_dimensions() == 2);
                lue_hpx_assert(static_cast<Count>(_hyperslab.count()[0]) == this->array_shape()[0]);
                lue_hpx_assert(static_cast<Count>(_hyperslab.count()[1]) == this->array_shape()[1]);
            }


            void do_preprocess() override
            {
                // Read (subset) from array whose pathname is passed in
                auto const& array_shape{this->array_shape()};
                auto const& partition_shape{this->partition_shape()};

                _flow_direction = read<FlowDirectionElement, 2>(
                    _array_pathname, _hyperslab, partition_shape, _object_id);
                _material = create_partitioned_array<MaterialElement>(array_shape, partition_shape, 1);
                _threshold = default_policies::uniform<MaterialElement>(array_shape, partition_shape, 5, 50);

                // We need to wait for all stuff that needs to be ready
                // before the calculations that need to be measured start.
                hpx::wait_all_n(_flow_direction.partitions().begin(), _flow_direction.nr_partitions());
                hpx::wait_all_n(_material.partitions().begin(), _material.nr_partitions());
                hpx::wait_all_n(_threshold.partitions().begin(), _threshold.nr_partitions());
            }


            void do_simulate(
                [[maybe_unused]] Count const time_step) override
            {
                // We are assuming a single time step is requested,
                // so feedback is not required
                /// lue_hpx_assert(time_step == 0);
                lue_hpx_assert(all_are_ready(_flow_direction));
                /// lue_hpx_assert(all_are_ready(this->state()));
                lue_hpx_assert(all_are_ready(_material));
                lue_hpx_assert(all_are_ready(_threshold));

                using Policies =
                    policy::accu_threshold::DefaultValuePolicies<FlowDirectionElement, MaterialElement>;

                std::tie(std::ignore, this->state()) = accu_threshold(
                    Policies{}, _flow_direction, _material, _threshold);
            }


        private:

            std::string _array_pathname;

            hdf5::Hyperslab _hyperslab;

            data_model::ID _object_id;

            PartitionedArray<FlowDirectionElement, 2> _flow_direction;

            PartitionedArray<MaterialElement, 2> _material;

            PartitionedArray<MaterialElement, 2> _threshold;

    };


}  // namespace lue::benchmark


namespace {

    std::string const usage = R"(
    usage:
        {} <flow_direction> [--center=<cell>]

    arguments:
        <flow_direction>  Pathname to array: dataset/phenomenon/property_set/layer

    options:
        --center=<cell>   Center of array to select: row_idx, col_idx

    Default center is the cell at the center of the array.
    )";

}  // Anonymous namespace


auto setup_benchmark(
    int argc,
    char* argv[],
    lue::benchmark::Environment const& environment,
    lue::benchmark::Task const& task)
{
    std::map<std::string, docopt::value> arguments{
        docopt::docopt(fmt::format(usage, argv[0]), {argv + 1, argv + argc}, true)};

    std::string const array_pathname{arguments.at("<flow_direction>").asString()};
    std::vector<lue::Index> center_cell;

    if(arguments.find("--center") != arguments.end())
    {
        std::string center_as_string{arguments.at("--center").asString()};

        // TODO Parse center_as_string: "row, col"
        // Can use regex
    }

    auto callable = [array_pathname, center_cell](
        lue::benchmark::Environment const& environment,
        lue::benchmark::Task const& task)
    {
        std::size_t const max_tree_depth = environment.max_tree_depth()
            ? *environment.max_tree_depth()
            : task.nr_time_steps();

        return lue::benchmark::AccuThresholdBenchmarkModel<float>{
            task, max_tree_depth, array_pathname, center_cell};
    };

    return lue::benchmark::ModelBenchmark<decltype(callable), lue::benchmark::AlgorithmBenchmarkResult>{
        std::move(callable), environment, task};
}


LUE_CONFIGURE_HPX_BENCHMARK()




                // - Generate a DEM of the requested shape
                // - Calculate a flow-direction field
                // - Generate a material field (input rain)
                // - Assign initial state (soil water)

                // using ElevationElement = double;
                // auto elevation = uniform<ElevationElement>(
                //     this->array_shape(), this->partition_shape(), 0, 1);

                // using FlowDirectionElement = ...;
                // _flow_direction = flow_direction_d8<FlowDirectionElement>(elevation);

                // using MaterialElement = ...;
                // _material = uniform(array_shape, partition_shape, min_material, max_material);

                // using ThresholdElement = ...;
                // _threshold = uniform(array_shape, partition_shape, min_threshold, max_threshold);

                // // TODO Verify the caller waits for the state to be ready
                // this->state() = _material;
                //
                //
                //
                // _material = uniform<MaterialElement>(array_shape, partition_shape, 10, 100);

                // // TODO Verify the caller waits for the state to be ready
                // this->state() = copy(_material);

