#include "copy.hpp"
#include "lue/framework/algorithm/copy.hpp"
#include "lue/framework/core/component/partitioned_array.hpp"


namespace lue {
namespace benchmark {
namespace detail {

template<
    typename Element,
    std::size_t rank>
void copy(
    std::uint64_t max_tree_depth,
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

    // → Create first array
    Array current_state{shape, partition_shape};

    for(std::size_t i = 0; i < task.nr_time_steps(); ++i) {

        // A very simple model: the new state is a copy of the current
        // state...
        // new_state is a newly allocated array containing array
        // partitions containing elements copied from current_state
        // → Create second array on each iteration. Moved from copy result.
        auto new_state = copy(current_state);

        // Prepare for the next iteration
        // The new current state is what is now the new state. The old
        // current state can be overwritten. If all goes well, partitions
        // not copied yet will remain in memory until they are not needed
        // anymore. Moving from a partitioned array should not explicitly
        // destruct component server instances.
        current_state = std::move(new_state);
    }
}

}  // namespace detail


void copy(
    std::uint64_t max_tree_depth,
    Task const& task)
{
    using Element = std::int32_t;

    switch(task.rank()) {
        case 2: {
            detail::copy<Element, 2>(max_tree_depth, task);
            break;
        }
    }

    // using Array = PartitionedArray<Element, rank>;
    // using Shape = typename Array::Shape;

    // // Allocate array for initial state. The array is uninitialized,
    // // which is fine.
    // Shape shape{{nr_rows, nr_cols}};
    // Array current_state{shape};

    // for(std::size_t i = 0; i < nr_time_steps; ++i) {

    //     // A very simple model: the new state is a copy of the current
    //     // state...
    //     // new_state is a newly allocated array containing array
    //     // partitions containing elements copied from current_state
    //     auto new_state = copy(current_state);

    //     // Prepare for the next iteration
    //     // The new current state is what is now the new state. The old
    //     // current state can be overwritten. If all goes well, partitions
    //     // not copied yet will remain in memory until they are not needed
    //     // anymore. Moving from a partitioned array should not explicitly
    //     // destruct component server instances.
    //     current_state = std::move(new_state);
    // }
}

}  // namespace benchmark
}  // namespace lue
