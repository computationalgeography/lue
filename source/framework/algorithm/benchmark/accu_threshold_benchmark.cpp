// #include "lue/framework/algorithm/pow.hpp"
// #include "lue/framework/algorithm/sqrt.hpp"
// #include "lue/framework/algorithm/uniform.hpp"
#include "lue/framework/benchmark/benchmark_model.hpp"
#include "lue/framework/benchmark/hpx_main.hpp"
#include "lue/framework/benchmark/model_benchmark.hpp"


namespace lue::benchmark {

    template<
        typename MaterialElement,
        Rank rank>
    class AccuThresholdBenchmarkModel final:
        public BenchmarkModel<MaterialElement, rank>
    {

        public:

            AccuThresholdBenchmarkModel(
                Task const& task,
                std::size_t max_tree_depth):

                BenchmarkModel<MaterialElement, rank>{task, max_tree_depth}

            {
            }


            void do_preprocess() override
            {
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
            }


            void do_simulate(
                [[maybe_unused]] Count const time_step) override
            {

                // std::tie(std::ignore, this->state() = accu_threshold(
                //     _flow_direction, this->state() + _material, _threshold);

                // this->state() = pow(sqrt(this->state()), Element{2});
            }

    };


}  // namespace lue::benchmark


auto setup_benchmark(
    int /* argc */,
    char* /* argv */[],
    lue::benchmark::Environment const& environment,
    lue::benchmark::Task const& task)
{
    auto callable = [](
        lue::benchmark::Environment const& environment,
        lue::benchmark::Task const& task)
    {
        std::size_t const max_tree_depth = environment.max_tree_depth()
            ? *environment.max_tree_depth()
            : task.nr_time_steps();

        return lue::benchmark::AccuThresholdBenchmarkModel<double, 2>{task, max_tree_depth};
    };

    return lue::benchmark::ModelBenchmark<decltype(callable), lue::benchmark::AlgorithmBenchmarkResult>{
        std::move(callable), environment, task};
}


LUE_CONFIGURE_HPX_BENCHMARK()
