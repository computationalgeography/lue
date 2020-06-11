#pragma once
#include "lue/framework/core/component/partitioned_array.hpp"
#include "lue/framework/benchmark/algorithm_benchmark_result.hpp"
#include "lue/framework/benchmark/format.hpp"
#include "lue/framework/benchmark/task.hpp"
#include "lue/framework/model/model.hpp"
#include <hpx/synchronization/sliding_semaphore.hpp>


namespace lue {
namespace benchmark {

template<
    typename Element,
    Rank rank>
class BenchmarkModel:
    // This class in inherited by benchmark models, which inherit
    // Model directly as well. We therefore need to use virtual
    // inheritance.
    public virtual Model
{

public:

    using Array = PartitionedArray<Element, rank>;

    using Shape = lue::Shape<Count, rank>;

    using Result = AlgorithmBenchmarkResult;

                   BenchmarkModel      (Task const& task,
                                        std::size_t max_tree_depth);

                   BenchmarkModel      (BenchmarkModel const&)=default;

                   BenchmarkModel      (BenchmarkModel&&)=default;

                   ~BenchmarkModel     ()=default;

    BenchmarkModel& operator=          (BenchmarkModel const&)=default;

    BenchmarkModel& operator=          (BenchmarkModel&&)=default;

    Shape const&   array_shape         () const;

    Shape const&   partition_shape     () const;

    void           set_result          (Result const& result);

    Result const&  result              () const;

    void           preprocess          () override;

    void           initialize          () override;

    void           simulate            (Count time_step) override;

    void           terminate           () override;

    void           postprocess         () override;

private:

    virtual void   do_preprocess       ();

    virtual void   do_initialize       ();

    virtual void   do_simulate         (Count time_step);

    virtual void   do_terminate        ();

    virtual void   do_postprocess      ();

    //! Number of runs performed
    Count _count;

    std::size_t    _max_tree_depth;

    hpx::lcos::local::sliding_semaphore _semaphore;

protected:

    Array _state;

private:

    Shape _array_shape;

    Shape _partition_shape;

    Result _result;

};


extern template class BenchmarkModel<std::int32_t, 2>;
extern template class BenchmarkModel<double, 2>;

}  // namespace benchmark
}  // namespace lue
