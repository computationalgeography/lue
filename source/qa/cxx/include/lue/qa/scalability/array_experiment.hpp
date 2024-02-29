#pragma once
#include "lue/qa/scalability/experiment.hpp"
#include <tuple>


namespace lue::qa {

    //! Shape type for representing the extends of an array (or partition)
    using Shape = std::tuple<Count, Count>;


    /*!
        @brief      The ArrayExperiment class represents an experiment where (2D) partitioned
                    arrays are used
    */
    class ArrayExperiment: public Experiment
    {

        public:

            ArrayExperiment(Count nr_workers, Shape const& array_shape, Shape const& partition_shape);

            ArrayExperiment(ArrayExperiment const&) = default;

            ArrayExperiment(ArrayExperiment&&) = default;

            ~ArrayExperiment() = default;

            auto operator=(ArrayExperiment const&) -> ArrayExperiment& = default;

            auto operator=(ArrayExperiment&&) -> ArrayExperiment& = default;

            [[nodiscard]] auto array_shape() const -> Shape const&;

            [[nodiscard]] auto partition_shape() const -> Shape const&;

        private:

            Shape _array_shape;

            Shape _partition_shape;
    };

}  // namespace lue::qa
