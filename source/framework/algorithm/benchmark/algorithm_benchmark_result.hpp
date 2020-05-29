#pragma once
#include "lue/framework/core/define.hpp"
#include <array>
#include <vector>


namespace lue {

class AlgorithmBenchmarkResult
{

public:

    using Shape = std::vector<Count>;

                   AlgorithmBenchmarkResult()=default;

    explicit       AlgorithmBenchmarkResult(
                                        Shape const& shape_in_partitions);

    template<
        Rank rank>
    explicit AlgorithmBenchmarkResult(
        std::array<Count, rank> const& shape_in_partitions):

        _shape_in_partitions{
            shape_in_partitions.begin(), shape_in_partitions.end()}

    {
    }

                   AlgorithmBenchmarkResult(AlgorithmBenchmarkResult const&)=default;

                   AlgorithmBenchmarkResult(AlgorithmBenchmarkResult&&)=default;

                   ~AlgorithmBenchmarkResult()=default;

    AlgorithmBenchmarkResult&
                   operator=(AlgorithmBenchmarkResult const&)=default;

    AlgorithmBenchmarkResult&
                   operator=(AlgorithmBenchmarkResult&&)=default;

    Shape const&   shape_in_partitions () const;

private:

    Shape _shape_in_partitions;

};

}  // namespace lue
