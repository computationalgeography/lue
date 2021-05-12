#include "lue/framework/algorithm/definition/pow.hpp"
#include "lue/framework/algorithm/sqrt.hpp"
#include "lue/framework/algorithm/uniform.hpp"
#include "lue/framework/benchmark/benchmark_model.hpp"
#include "lue/framework/benchmark/hpx_main.hpp"
#include "lue/framework/benchmark/model_benchmark.hpp"


namespace lue {
namespace benchmark {

template<
    typename Element,
    Rank rank>
class SqrtBenchmarkModel final:
    public BenchmarkModel<Element, rank>
{

public:

                   SqrtBenchmarkModel  (Task const& task,
                                        std::size_t max_tree_depth);

                   SqrtBenchmarkModel  (SqrtBenchmarkModel const&)=default;

                   SqrtBenchmarkModel  (SqrtBenchmarkModel&&)=default;

                   ~SqrtBenchmarkModel ()=default;

    SqrtBenchmarkModel&
                   operator=           (SqrtBenchmarkModel const&)=default;

    SqrtBenchmarkModel&
                   operator=           (SqrtBenchmarkModel&&)=default;

    void           do_preprocess       () override;

    void           do_simulate         (Count time_step) override;

private:

};


template<
    typename Element,
    std::size_t rank>
SqrtBenchmarkModel<Element, rank>::SqrtBenchmarkModel(
    Task const& task,
    std::size_t const max_tree_depth):

    BenchmarkModel<Element, rank>{task, max_tree_depth}

{
}


template<
    typename Element,
    std::size_t rank>
void SqrtBenchmarkModel<Element, rank>::do_preprocess()
{
    this->state() = uniform(this->state(), Element{0}, std::numeric_limits<Element>::max());
}


template<
    typename Element,
    std::size_t rank>
void SqrtBenchmarkModel<Element, rank>::do_simulate(
    Count const /* time_step */)
{
    this->state() = pow(sqrt(this->state()), Element{2});
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

        return lue::benchmark::SqrtBenchmarkModel<double, 2>{task, max_tree_depth};
    };

    return lue::benchmark::ModelBenchmark<decltype(callable), lue::benchmark::AlgorithmBenchmarkResult>{
        std::move(callable), environment, task};
}


LUE_CONFIGURE_HPX_BENCHMARK()
