#pragma once
#include "algorithm_benchmark_result.hpp"
#include "format.hpp"
#include "lue/framework/core/shape.hpp"
#include "lue/framework/model/model.hpp"
#include "lue/framework/benchmark/task.hpp"


namespace lue {
namespace benchmark {

template<
    Rank rank>
class BenchmarkModel:
    public Model
{

public:

    using Shape = lue::Shape<Count, rank>;

    using Result = AlgorithmBenchmarkResult;

                   BenchmarkModel      (Task const& task);

                   BenchmarkModel      (BenchmarkModel const&)=default;

                   BenchmarkModel      (BenchmarkModel&&)=default;

                   ~BenchmarkModel     ()=default;

    BenchmarkModel& operator=          (BenchmarkModel const&)=default;

    BenchmarkModel& operator=          (BenchmarkModel&&)=default;

    Shape const&   array_shape         () const;

    Shape const&   partition_shape     () const;

    void           set_result          (Result const& result);

    Result const&  result              () const;

private:

    Shape _array_shape;

    Shape _partition_shape;

    Result  _result;

};


extern template class BenchmarkModel<2>;

}  // namespace benchmark
}  // namespace lue
