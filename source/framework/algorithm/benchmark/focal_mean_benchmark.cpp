#include "lue/framework/algorithm/definition/focal_mean.hpp"
#include "lue/framework/algorithm/kernel.hpp"
#include "lue/framework/algorithm/default_policies/uniform.hpp"
#include "lue/framework/algorithm/serialize/kernel.hpp"
#include "lue/framework/benchmark/benchmark_model.hpp"
#include "lue/framework/benchmark/hpx_main.hpp"
#include "lue/framework/benchmark/model_benchmark.hpp"


namespace lue {
namespace benchmark {

template<
    typename Element,
    Rank rank>
class FocalMeanBenchmarkModel final:
    public BenchmarkModel<Element, rank>
{

public:

    using Kernel = lue::Kernel<bool, rank>;

                   FocalMeanBenchmarkModel(
                                        Task const& task,
                                        std::size_t max_tree_depth);

                   FocalMeanBenchmarkModel(
                                        FocalMeanBenchmarkModel const&)=default;

                   FocalMeanBenchmarkModel(
                                        FocalMeanBenchmarkModel&&)=default;

                   ~FocalMeanBenchmarkModel()=default;

    FocalMeanBenchmarkModel&
                   operator=           (FocalMeanBenchmarkModel const&)=default;

    FocalMeanBenchmarkModel&
                   operator=           (FocalMeanBenchmarkModel&&)=default;

    void           do_preprocess       () override;

    void           do_simulate         (Count time_step) override;

private:

    Kernel _kernel;

};


template<
    typename Element,
    std::size_t rank>
FocalMeanBenchmarkModel<Element, rank>::FocalMeanBenchmarkModel(
    Task const& task,
    std::size_t const max_tree_depth):

    BenchmarkModel<Element, rank>{task, max_tree_depth},
    _kernel{}

{
}


template<
    typename Element,
    std::size_t rank>
void FocalMeanBenchmarkModel<Element, rank>::do_preprocess()
{
    this->state() = default_policies::uniform(this->state(), Element{0}, std::numeric_limits<Element>::max());
    _kernel = lue::box_kernel<bool, rank>(1, true);
}


template<
    typename Element,
    std::size_t rank>
void FocalMeanBenchmarkModel<Element, rank>::do_simulate(
    Count const /* time_step */)
{
    this->state() = focal_mean(this->state(), _kernel);
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

        return lue::benchmark::FocalMeanBenchmarkModel<double, 2>{task, max_tree_depth};
    };

    return lue::benchmark::ModelBenchmark<decltype(callable), lue::benchmark::AlgorithmBenchmarkResult>{
        std::move(callable), environment, task};
}


LUE_CONFIGURE_HPX_BENCHMARK()
