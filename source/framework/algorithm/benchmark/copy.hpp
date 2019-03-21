#pragma once
#include <cstdint>


namespace lue {
namespace benchmark {

void               copy                (std::uint64_t max_tree_depth,
                                        std::uint64_t nr_time_steps,
                                        std::uint64_t nr_rows,
                                        std::uint64_t nr_cols,
                                        std::uint64_t nr_rows_partition,
                                        std::uint64_t nr_cols_partition);

}  // namespace benchmark
}  // namespace lue
