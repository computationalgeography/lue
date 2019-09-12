#include "copy.hpp"
#include "lue/framework/core/component/partitioned_array.hpp"
#include "lue/framework/algorithm/copy.hpp"
#include <hpx/include/iostreams.hpp>


namespace lue {
namespace benchmark {
namespace detail {

template<
    typename Element,
    std::size_t rank>
void copy(
    Task const& task)
{
    using Array = PartitionedArray<Element, rank>;
    using Shape = typename Array::Shape;

    // Allocate array for initial state. The array is uninitialized,
    // which is fine.
    Shape shape;
    std::copy(
        task.array_shape().begin(), task.array_shape().end(),
        shape.begin());

    Shape partition_shape;
    std::copy(
        task.partition_shape().begin(), task.partition_shape().end(),
        partition_shape.begin());

    // → Create initial array
    Array state{shape, partition_shape};
    hpx::cout << describe(state) << hpx::endl;

    assert(state.shape() == shape);

    for(std::size_t i = 0; i < task.nr_time_steps(); ++i) {

        // A very simple model: the new state is a copy of the current
        // state...
        state = copy(state);

        hpx::cout << '.' << hpx::flush;
    }

    hpx::cout << "!" << hpx::flush;

    hpx::wait_all_n(state.begin(), state.nr_partitions());

    hpx::cout << hpx::endl;
}

}  // namespace detail


void copy(
    Task const& task)
{
    using Element = std::int32_t;

    switch(task.rank()) {
        case 2: {
            detail::copy<Element, 2>(task);
            break;
        }
    }
}

}  // namespace benchmark
}  // namespace lue
