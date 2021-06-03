#include "lue/framework/benchmark/task.hpp"
#include "lue/framework/core/assert.hpp"
#include <functional>
#include <numeric>


namespace lue {
namespace benchmark {

Task::Task(
    std::uint64_t const nr_time_steps,
    Shape const& array_shape,
    Shape const& partition_shape):

    _nr_time_steps{nr_time_steps},
    _array_shape{array_shape},
    _partition_shape{partition_shape}

{
    lue_hpx_assert(_nr_time_steps > 0);
    lue_hpx_assert(_array_shape.size() == _partition_shape.size());
}


std::uint64_t Task::nr_time_steps() const
{
    return _nr_time_steps;
}


std::size_t Task::rank() const
{
    return _array_shape.size();
}


std::uint64_t Task::nr_elements() const
{
    lue_hpx_assert(!_array_shape.empty());

    return std::accumulate(
        _array_shape.begin(), _array_shape.end(), std::uint64_t{1},
        std::multiplies<typename Shape::value_type>());
}


Task::Shape const& Task::array_shape() const
{
    return _array_shape;
}


Task::Shape const& Task::partition_shape() const
{
    return _partition_shape;
}

}  // namespace benchmark
}  // namespace lue
