#include "lue/qa/scalability/array_experiment.hpp"


namespace lue::qa {

    ArrayExperiment::ArrayExperiment(
        Count const nr_workers, Shape const& array_shape, Shape const& partition_shape):

        Experiment{nr_workers},
        _array_shape{array_shape},
        _partition_shape{partition_shape}

    {
    }


    Shape const& ArrayExperiment::array_shape() const
    {
        return _array_shape;
    }


    Shape const& ArrayExperiment::partition_shape() const
    {
        return _partition_shape;
    }

}  // namespace lue::qa
