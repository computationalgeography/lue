#include "lue/framework/algorithm/pow.hpp"
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
    this->_state = uniform(this->_state, Element{0}, std::numeric_limits<Element>::max());
}


template<
    typename Element,
    std::size_t rank>
void SqrtBenchmarkModel<Element, rank>::do_simulate(
    Count const /* time_step */)
{
    this->_state = pow(sqrt(this->_state), Element{2});
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
