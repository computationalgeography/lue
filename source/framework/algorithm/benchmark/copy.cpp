#include "copy.hpp"
#include "lue/framework/algorithm/copy.hpp"
#include "lue/framework/core/component/partitioned_array.hpp"


namespace lue {
namespace benchmark {

void copy(
    std::uint64_t max_tree_depth,
    std::uint64_t nr_time_steps,
    std::uint64_t nr_rows,
    std::uint64_t nr_cols,
    std::uint64_t nr_rows_partition,
    std::uint64_t nr_cols_partition)
{
    using Element = std::int32_t;
    std::size_t const rank = 2;
    using Array = PartitionedArray<Element, rank>;
    using Shape = typename Array::Shape;

    // Allocate array for initial state. The array is uninitialized,
    // which is fine.
    Shape shape{{nr_rows, nr_cols}};
    Array current_state{shape};

    for(std::size_t i = 0; i < nr_time_steps; ++i) {

        // A very simple model: the new state is a copy of the current
        // state...
        // new_state is a newly allocated array containing array
        // partitions containing elements copied from current_state
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

}  // namespace benchmark
}  // namespace lue
