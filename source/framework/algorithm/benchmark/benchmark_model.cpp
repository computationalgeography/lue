#include "benchmark_model.hpp"
#include <algorithm>


namespace lue {
namespace benchmark {

template<
    Rank rank>
BenchmarkModel<rank>::BenchmarkModel(
    Task const& task):

    Model{},
    _array_shape{},
    _partition_shape{}

{
    std::copy(
        task.array_shape().begin(), task.array_shape().end(),
        _array_shape.begin());

    std::copy(
        task.partition_shape().begin(), task.partition_shape().end(),
        _partition_shape.begin());
}


template<
    Rank rank>
typename BenchmarkModel<rank>::Shape const&
    BenchmarkModel<rank>::array_shape() const
{
    return _array_shape;
}


template<
    Rank rank>
typename BenchmarkModel<rank>::Shape const&
    BenchmarkModel<rank>::partition_shape() const
{
    return _partition_shape;
}


template class BenchmarkModel<2>;

}  // namespace benchmark
}  // namespace lue
