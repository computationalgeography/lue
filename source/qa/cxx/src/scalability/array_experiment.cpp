#include "lue/qa/scalability/array_experiment.hpp"


namespace lue::qa {

    /*!
        @brief      Construct and instance for experiment using @a nr_workers and with arrays of shape
                    @a arrays_shape and partitioned using @a partition_shape
    */
    ArrayExperiment::ArrayExperiment(
        Count const nr_workers, Shape const& array_shape, Shape const& partition_shape):

        Experiment{nr_workers},
        _array_shape{array_shape},
        _partition_shape{partition_shape}

    {
    }


    /*!
        @brief      Return the array shape
    */
    auto ArrayExperiment::array_shape() const -> Shape const&
    {
        return _array_shape;
    }


    /*!
        @brief      Return the partition shape
    */
    auto ArrayExperiment::partition_shape() const -> Shape const&
    {
        return _partition_shape;
    }

}  // namespace lue::qa
