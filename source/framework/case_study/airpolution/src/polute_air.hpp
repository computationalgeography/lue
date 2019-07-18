#pragma once
#include "lue/framework/core/shape.hpp"
#include <string>
#include <vector>


namespace lue {

// void               polute_air          (std::string const& dataset_pathname,
//                                         std::uint64_t nr_time_steps,
//                                         std::uint64_t nr_partitions);

void               polute_air          (std::uint64_t max_tree_depth,
                                        std::uint64_t nr_time_steps,
                                        Shape<std::uint64_t, 2> const&
                                            array_shape,
                                        Shape<std::uint64_t, 2> const&
                                            partition_shape);

}  // namespace lue
