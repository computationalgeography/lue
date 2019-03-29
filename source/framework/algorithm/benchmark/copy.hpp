#pragma once
#include <cstdint>
#include <vector>
#include "lue/framework/benchmark/task.hpp"


namespace lue {
namespace benchmark {

void               copy                (std::uint64_t max_tree_depth,
                                        Task const& task);

}  // namespace benchmark
}  // namespace lue
