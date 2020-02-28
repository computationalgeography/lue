#pragma once
#include "lue/framework/benchmark/task.hpp"


namespace lue {
namespace benchmark {

void               change_land_use     (Task const& task,
                                        std::size_t max_tree_depth);

}  // namespace benchmark
}  // namespace lue
