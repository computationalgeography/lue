#include "lue/framework/benchmark/format.hpp"


namespace lue {
namespace benchmark {

void to_json(
    nlohmann::json& json,
    AlgorithmBenchmarkResult const& result)
{
    json = nlohmann::json{
            {
                "shape_in_partitions", result.shape_in_partitions()
            }
        };
}

}  // namespace benchmark
}  // namespace lue
