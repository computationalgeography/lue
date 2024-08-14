#pragma once
#include "lue/framework/core/shape.hpp"
#include <string>
#include <vector>


namespace lue {

    // void               pollute_air          (std::string const& dataset_pathname,
    //                                         std::uint64_t nr_time_steps,
    //                                         std::uint64_t nr_partitions);

    void pollute_air(
        lue::Count nr_time_steps,
        Shape<lue::Count, 2> const& array_shape,
        Shape<lue::Count, 2> const& partition_shape);

}  // namespace lue
