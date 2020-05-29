#include "benchmark_model.hpp"
#include "lue/framework/algorithm/arithmetic.hpp"
#include "lue/framework/algorithm/sqrt.hpp"
#include "lue/framework/algorithm/uniform.hpp"
#include "lue/framework/benchmark/hpx_main.hpp"
#include "lue/framework/benchmark/model_benchmark.hpp"


namespace lue {
namespace benchmark {

template<
    typename Element,
    Rank rank>
class SqrtBenchmarkModel final:
    public BenchmarkModel<rank>
{

public:

    using Array = PartitionedArray<Element, rank>;

                   SqrtBenchmarkModel  (Task const& task);

                   SqrtBenchmarkModel  (SqrtBenchmarkModel const&)=default;

                   SqrtBenchmarkModel  (SqrtBenchmarkModel&&)=default;

                   ~SqrtBenchmarkModel ()=default;

    SqrtBenchmarkModel&
                   operator=           (SqrtBenchmarkModel const&)=default;

    SqrtBenchmarkModel&
                   operator=           (SqrtBenchmarkModel&&)=default;

    void           preprocess          ();

    void           initialize          ();

    void           simulate            (Count time_step);

    void           terminate           ();

    void           postprocess         ();

private:

    Array _state;

};


template<
    typename Element,
    std::size_t rank>
SqrtBenchmarkModel<Element, rank>::SqrtBenchmarkModel(
    Task const& task):

    BenchmarkModel<rank>{task},
    _state{}

{
}


template<
    typename Element,
    std::size_t rank>
void SqrtBenchmarkModel<Element, rank>::preprocess()
{
    _state = uniform(
        Array{this->array_shape(), this->partition_shape()},
        Element{0}, std::numeric_limits<Element>::max());

    lue_assert(_state.shape() == this->array_shape());

    hpx::cout << describe(_state) << hpx::endl;
}


template<
    typename Element,
    std::size_t rank>
void SqrtBenchmarkModel<Element, rank>::initialize()
{
    hpx::cout << '[' << hpx::flush;
}


template<
    typename Element,
    std::size_t rank>
void SqrtBenchmarkModel<Element, rank>::simulate(
    Count const /* time_step */)
{
    _state = sqrt(_state) * sqrt(_state);
    hpx::cout << '.' << hpx::flush;
}


template<
    typename Element,
    std::size_t rank>
void SqrtBenchmarkModel<Element, rank>::terminate()
{
    hpx::wait_all_n(_state.begin(), _state.nr_partitions());
    hpx::cout << "]\n" << hpx::flush;
}


template<
    typename Element,
    std::size_t rank>
void SqrtBenchmarkModel<Element, rank>::postprocess()
{
    this->set_result(AlgorithmBenchmarkResult{_state.partitions().shape()});
}


// namespace detail {
// 
// template<
//     typename Element,
//     std::size_t rank>
// AlgorithmBenchmarkResult sqrt(
//     Task const& task,
//     std::size_t const max_tree_depth)
// {
//     lue_assert(max_tree_depth > 0);
// 
//     using Array = PartitionedArray<Element, rank>;
//     using Shape = typename Array::Shape;
// 
//     Shape shape;
//     std::copy(
//         task.array_shape().begin(), task.array_shape().end(),
//         shape.begin());
// 
//     Shape partition_shape;
//     std::copy(
//         task.partition_shape().begin(), task.partition_shape().end(),
//         partition_shape.begin());
// 
//     // → Create initial array
//     Array state{shape, partition_shape};
//     hpx::cout << describe(state) << hpx::endl;
// 
//     AlgorithmBenchmarkResult result{state.partitions().shape()};
// 
//     lue_assert(state.shape() == shape);
// 
//     state = uniform(state, Element{0}, std::numeric_limits<Element>::max());
// 
//     hpx::lcos::local::sliding_semaphore semaphore{
//         static_cast<std::int64_t>(max_tree_depth)};
// 
//     for(std::size_t i = 0; i < task.nr_time_steps(); ++i) {
// 
//         // Wait if there are more than max_tree_depth iterations in flight
//         semaphore.wait(i);
// 
//         state = sqrt(state) * sqrt(state);
// 
//         hpx::cout << '.' << hpx::flush;
// 
//         // Attach a continuation to the state at the current time
//         // step. Once it is finished, signal the semaphore so it knowns
//         // that we can have another iteration in flight.
//         hpx::when_all_n(state.begin(), state.nr_partitions()).then(
//             hpx::launch::sync,
//             [&semaphore, i](
//                 auto const&)
//             {
//                 semaphore.signal(i);
//             });
//     }
// 
//     hpx::cout << "!" << hpx::flush;
// 
//     hpx::wait_all_n(state.begin(), state.nr_partitions());
// 
//     hpx::cout << hpx::endl;
// 
//     return result;
// }
// 
// }  // namespace detail
// 
// 
// AlgorithmBenchmarkResult sqrt(
//     Task const& task,
//     std::size_t const max_tree_depth)
// {
//     using Element = double;
// 
//     AlgorithmBenchmarkResult result;
// 
//     switch(task.rank()) {
//         case 2: {
//             result = detail::sqrt<Element, 2>(task, max_tree_depth);
//             break;
//         }
//     }
// 
//     return result;
// }

}  // namespace benchmark
}  // namespace lue


// auto setup_benchmark(
//         int /* argc */,
//         char* /* argv */[],
//         lue::benchmark::Environment const& environment,
//         lue::benchmark::Task const& task)
// {
//     // This function is called on the main locality. Return a callable that
//     // performs the work to benchmark.
// 
//     // Function to benchmark
//     auto callable = [](
//         lue::benchmark::Environment const& environment,
//         lue::benchmark::Task const& task) -> lue::AlgorithmBenchmarkResult
//     {
//         std::size_t const max_tree_depth = environment.max_tree_depth()
//             ? *environment.max_tree_depth()
//             : task.nr_time_steps();
// 
//         return lue::benchmark::sqrt(task, max_tree_depth);
//     };
// 
//     return lue::benchmark::Benchmark<
//             decltype(callable), lue::AlgorithmBenchmarkResult>{
//         std::move(callable), environment, task};
// }


auto setup_benchmark(
    int /* argc */,
    char* /* argv */[],
    lue::benchmark::Environment const& environment,
    lue::benchmark::Task const& task)
{
    auto callable = [](
        lue::benchmark::Environment const& /* environment */,
        lue::benchmark::Task const& task)
    {
        return lue::benchmark::SqrtBenchmarkModel<double, 2>{task};
    };

    return lue::benchmark::ModelBenchmark<
            decltype(callable), lue::AlgorithmBenchmarkResult>{
        std::move(callable), environment, task};
}


LUE_CONFIGURE_HPX_BENCHMARK()
