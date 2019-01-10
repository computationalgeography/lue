#pragma once
#include <string>


namespace lue {

// void               polute_air          (std::string const& dataset_pathname,
//                                         std::uint64_t nr_time_steps,
//                                         std::uint64_t nr_partitions);

void               polute_air          (std::uint64_t nr_time_steps,
                                        std::uint64_t nr_rows,
                                        std::uint64_t nr_cols,
                                        std::uint64_t nr_rows_partition,
                                        std::uint64_t nr_cols_partition);

}  // namespace lue
