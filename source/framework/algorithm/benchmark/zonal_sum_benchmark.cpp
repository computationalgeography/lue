#include "lue/framework/algorithm/uniform.hpp"
#include "lue/framework/algorithm/zonal_sum.hpp"
#include "lue/framework/benchmark/benchmark_model.hpp"
#include "lue/framework/benchmark/hpx_main.hpp"
#include "lue/framework/benchmark/model_benchmark.hpp"


namespace lue {
namespace benchmark {

template<
    typename Element,
    Rank rank>
class ZonalSumBenchmark:
    public BenchmarkModel<Element, rank>
{

public:

    using Zone = std::int32_t;

    using Zones = PartitionedArray<Zone, rank>;

                   ZonalSumBenchmark   (Task const& task,
                                        std::size_t max_tree_depth);

                   ZonalSumBenchmark   (ZonalSumBenchmark const&)=default;

                   ZonalSumBenchmark   (ZonalSumBenchmark&&)=default;

                   ~ZonalSumBenchmark  ()=default;

    ZonalSumBenchmark&
                   operator=           (ZonalSumBenchmark const&)=default;

    ZonalSumBenchmark&
                   operator=           (ZonalSumBenchmark&&)=default;

    void           do_preprocess       () override;

    void           do_simulate         (Count time_step) override;

private:

    Zone _max_zone;

    Zones _zones;

};


template<
    typename Element,
    std::size_t rank>
ZonalSumBenchmark<Element, rank>::ZonalSumBenchmark(
    Task const& task,
    std::size_t const max_tree_depth):

    BenchmarkModel<Element, rank>{task, max_tree_depth},
    _max_zone{100},
    _zones{}

{
}


template<
    typename Element,
    std::size_t rank>
void ZonalSumBenchmark<Element, rank>::do_preprocess()
{
    static_assert(std::is_floating_point_v<Element>);

    this->_state = uniform(this->_state, Element{-1e6}, Element{1e6});

    _zones = Zones{this->array_shape(), this->partition_shape()};
    _zones = uniform(_zones, Zone{0}, _max_zone);
}


template<
    typename Element,
    std::size_t rank>
void ZonalSumBenchmark<Element, rank>::do_simulate(
    Count const /* time_step */)
{
    // Aggregate per currently distributed zones
    this->_state = zonal_sum(this->_state, _zones);

    // Redistribute zones
    _zones = uniform(_zones, Zone{0}, _max_zone);
}

}  // namespace benchmark
}  // namespace lue


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

        return lue::benchmark::ZonalSumBenchmark<double, 2>{task, max_tree_depth};
    };

    return lue::benchmark::ModelBenchmark<decltype(callable), lue::benchmark::AlgorithmBenchmarkResult>{
        std::move(callable), environment, task};
}


LUE_CONFIGURE_HPX_BENCHMARK()
