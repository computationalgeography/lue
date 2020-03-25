#pragma once
#include "algorithm_benchmark_result.hpp"
#include <nlohmann/json.hpp>


namespace lue {

void               to_json             (nlohmann::json& json,
                                        AlgorithmBenchmarkResult const& result);

}  // namespace lue
