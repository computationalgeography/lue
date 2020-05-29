#include "algorithm_benchmark_result.hpp"


namespace lue {

AlgorithmBenchmarkResult::AlgorithmBenchmarkResult(
    Shape const& shape_in_partitions):

    _shape_in_partitions{shape_in_partitions}

{
}


AlgorithmBenchmarkResult::Shape const&
    AlgorithmBenchmarkResult::shape_in_partitions() const
{
    return _shape_in_partitions;
}

}  // namespace lue
