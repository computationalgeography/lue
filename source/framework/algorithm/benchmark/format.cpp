#include "format.hpp"


namespace lue {

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

}  // namespace lue
