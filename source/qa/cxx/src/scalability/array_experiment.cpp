#include "lue/qa/scalability/array_experiment.hpp"


namespace lue::qa {

    ArrayExperiment::ArrayExperiment(
        Count const nr_workers, Shape const& array_shape, Shape const& partition_shape):

        Experiment{nr_workers},
        _array_shape{array_shape},
        _partition_shape{partition_shape}

    {
    }


    auto ArrayExperiment::array_shape() const -> Shape const&
    {
        return _array_shape;
    }


    auto ArrayExperiment::partition_shape() const -> Shape const&
    {
        return _partition_shape;
    }

}  // namespace lue::qa
