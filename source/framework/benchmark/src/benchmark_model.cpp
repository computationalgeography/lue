#include "lue/framework/benchmark/benchmark_model.hpp"
#include "lue/framework/algorithm/create_partitioned_array.hpp"
#include "lue/framework/core/component.hpp"
#include <hpx/iostream.hpp>
#include <algorithm>


namespace lue {
namespace benchmark {

template<
    typename Element,
    Rank rank>
BenchmarkModel<Element, rank>::BenchmarkModel(
    Task const& task,
    std::size_t const max_tree_depth):

    Model{},
    _count{0},
    _max_tree_depth{max_tree_depth},
    _semaphore{static_cast<std::int64_t>(_max_tree_depth)},
    _state_ptr{std::make_shared<Array>()},
    _array_shape{},
    _partition_shape{}

{
    lue_hpx_assert(_state_ptr);

    std::copy(
        task.array_shape().begin(), task.array_shape().end(),
        _array_shape.begin());

    std::copy(
        task.partition_shape().begin(), task.partition_shape().end(),
        _partition_shape.begin());
}


template<
    typename Element,
    Rank rank>
typename BenchmarkModel<Element, rank>::Shape const&
    BenchmarkModel<Element, rank>::array_shape() const
{
    return _array_shape;
}


template<
    typename Element,
    Rank rank>
typename BenchmarkModel<Element, rank>::Shape const&
    BenchmarkModel<Element, rank>::partition_shape() const
{
    return _partition_shape;
}


template<
    typename Element,
    Rank rank>
void BenchmarkModel<Element, rank>::set_result(
    Result const& result)
{
    lue_hpx_assert(result.shape_in_partitions().size() == rank);

    _result = result;
}


template<
    typename Element,
    Rank rank>
typename BenchmarkModel<Element, rank>::Result const&
    BenchmarkModel<Element, rank>::result() const
{
    lue_hpx_assert(_result.shape_in_partitions().size() == rank);

    return _result;
}


template<
    typename Element,
    Rank rank>
void BenchmarkModel<Element, rank>::preprocess()
{
    state() = create_partitioned_array<Element>(this->array_shape(), this->partition_shape());

    do_preprocess();

    // Time spent calculating state at t0 is not part of the
    // benchmark. Compare this to reading from a dataset. Leave I/O out.
    hpx::wait_all_n(state().partitions().begin(), state().nr_partitions());

    if(_count == 0)
    {
        hpx::cout << describe(state()) << hpx::endl;
    }
}


template<
    typename Element,
    Rank rank>
void BenchmarkModel<Element, rank>::initialize()
{
    hpx::cout << '[' << hpx::flush;

    do_initialize();
}


template<
    typename Element,
    Rank rank>
void BenchmarkModel<Element, rank>::simulate(
    Count const time_step)
{
    do_simulate(time_step);

    hpx::cout << '.' << hpx::flush;

    // Every _max_tree_depth time steps, attach additional continuation
    // which will trigger the semaphore once computation has reached
    // this point
    if(((time_step + 1) % _max_tree_depth) == 0)
    {
        state().partitions()[0].then(

            [this, time_step](auto const&)
            {
                // Inform semaphore about new lower limit
                _semaphore.signal(time_step);
            });
    }

    // We are creating tasks for the current time step, at the front of
    // the task tree. Suspend if the difference between the current time
    // step and the time step for which tasks are currently being executed
    // is larger than the max_tree_depth.
    //
    // The continuation above will resume this thread once the computation
    // has caught up
    _semaphore.wait(time_step);
}


template<
    typename Element,
    Rank rank>
void BenchmarkModel<Element, rank>::terminate()
{
    do_terminate();

    hpx::wait_all_n(state().partitions().begin(), state().nr_partitions());

    lue_hpx_assert(all_are_ready(state().partitions()));
    lue_hpx_assert(none_have_exception(state().partitions()));

    hpx::cout << "]\n" << hpx::flush;
}


template<
    typename Element,
    Rank rank>
void BenchmarkModel<Element, rank>::postprocess()
{
    do_postprocess();

    this->set_result(AlgorithmBenchmarkResult{state().partitions().shape()});

    ++_count;
}


template<
    typename Element,
    Rank rank>
void BenchmarkModel<Element, rank>::do_preprocess()
{
}


template<
    typename Element,
    Rank rank>
void BenchmarkModel<Element, rank>::do_initialize()
{
}


template<
    typename Element,
    Rank rank>
void BenchmarkModel<Element, rank>::do_simulate(
    Count const /* time_step */)
{
}


template<
    typename Element,
    Rank rank>
void BenchmarkModel<Element, rank>::do_terminate()
{
}


template<
    typename Element,
    Rank rank>
void BenchmarkModel<Element, rank>::do_postprocess()
{
}


template class BenchmarkModel<std::int32_t, 2>;
template class BenchmarkModel<float, 2>;
template class BenchmarkModel<double, 2>;

}  // namespace benchmark
}  // namespace lue
