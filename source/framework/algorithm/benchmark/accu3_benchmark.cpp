#include "lue/framework/algorithm/value_policies/accu3.hpp"
#include "lue/framework/benchmark/benchmark_model.hpp"
#include "lue/framework/benchmark/data_model.hpp"
#include "lue/framework/benchmark/hpx_main.hpp"
#include "lue/framework/benchmark/model_benchmark.hpp"
#include "lue/framework/benchmark/regex.hpp"
#include "lue/framework/io/read_into.hpp"
#include <docopt/docopt.h>


namespace lue::benchmark {

    template<
        typename MaterialElement>
    class AccuBenchmarkModel final:
        public BenchmarkModel<MaterialElement, 2>
    {

        private:

               using FlowDirectionElement = std::uint8_t;

        public:

            AccuBenchmarkModel(
                Task const& task,
                std::size_t max_tree_depth,
                std::string const& array_pathname,
                std::vector<Index> center_cell):

                BenchmarkModel<MaterialElement, 2>{task, max_tree_depth},
                _array_pathname{array_pathname},
                _object_id{},
                _hyperslab{}

            {
                std::tie(_object_id, _hyperslab) = array_info(array_pathname, center_cell, this->array_shape());
            }


            void do_preprocess() override
            {
                // Read (subset) from array whose pathname is passed in
                auto const& array_shape{this->array_shape()};
                auto const& partition_shape{this->partition_shape()};

                // 1 byte / cell
                _flow_direction = read<FlowDirectionElement, 2>(
                    _array_pathname, _hyperslab, partition_shape, _object_id);
                // 4 bytes / cell
                _material = create_partitioned_array<MaterialElement>(array_shape, partition_shape, 1);

                // We need to wait for all stuff that needs to be ready
                // before the calculations that need to be measured start.
                hpx::wait_all_n(_flow_direction.partitions().begin(), _flow_direction.nr_partitions());
                hpx::wait_all_n(_material.partitions().begin(), _material.nr_partitions());
            }


            void do_simulate(
                [[maybe_unused]] Count const time_step) override
            {
                // We are assuming a single time step is requested,
                // so feedback is not required
                lue_hpx_assert(all_are_ready(_flow_direction));
                lue_hpx_assert(all_are_ready(_material));

                // 4 bytes / cell
                this->state() = value_policies::accu3(_flow_direction, _material);
            }


        private:

            std::string _array_pathname;

            data_model::ID _object_id;

            hdf5::Hyperslab _hyperslab;

            PartitionedArray<FlowDirectionElement, 2> _flow_direction;

            PartitionedArray<MaterialElement, 2> _material;

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
        center_cell = lue::benchmark::parse_idxs(arguments.at("--center").asString());
    }


    auto callable = [array_pathname, center_cell](
        lue::benchmark::Environment const& environment,
        lue::benchmark::Task const& task)
    {
        std::size_t const max_tree_depth = environment.max_tree_depth()
            ? *environment.max_tree_depth()
            : task.nr_time_steps();

        return lue::benchmark::AccuBenchmarkModel<float>{
            task, max_tree_depth, array_pathname, center_cell};
    };

    return lue::benchmark::ModelBenchmark<decltype(callable), lue::benchmark::AlgorithmBenchmarkResult>{
        std::move(callable), environment, task};
}


LUE_CONFIGURE_HPX_BENCHMARK()
