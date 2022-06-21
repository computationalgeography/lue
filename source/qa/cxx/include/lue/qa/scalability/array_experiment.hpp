#pragma once
#include "lue/qa/scalability/experiment.hpp"
#include <tuple>


namespace lue::qa {

    using Shape = std::tuple<Count, Count>;


    class ArrayExperiment:

        public Experiment

    {

        public:

            ArrayExperiment(
                Count const nr_workers,
                Shape const& array_shape,
                Shape const& partition_shape);

            ArrayExperiment(ArrayExperiment const&)=default;

            ArrayExperiment(ArrayExperiment&&)=default;

            ~ArrayExperiment()=default;

            ArrayExperiment& operator=(ArrayExperiment const&)=default;

            ArrayExperiment& operator=(ArrayExperiment&&)=default;

            Shape const& array_shape() const;

            Shape const& partition_shape() const;

        private:

            Shape _array_shape;

            Shape _partition_shape;

    };

}  // namespace lue::qa
