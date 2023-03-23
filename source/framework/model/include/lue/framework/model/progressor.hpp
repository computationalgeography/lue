#pragma once
#include "lue/framework/core/define.hpp"


namespace lue {

    /*!
        @brief      Base class for progressors

        When a model is executed, progressors can be used to visualize the progress.
    */
    class Progressor
    {

        public:

            Progressor() = default;

            Progressor(Progressor const&) = default;

            Progressor(Progressor&&) = default;

            virtual ~Progressor() = default;

            Progressor& operator=(Progressor const&) = default;

            Progressor& operator=(Progressor&&) = default;

            virtual void preprocess(Count const sample_nr);

            virtual void initialize();

            virtual void simulate(Count const time_step);

            virtual void finalize();

            virtual void postprocess();

        private:
    };


    void preprocess(Progressor& progressor, Count const sample_nr);

    void initialize(Progressor& progressor);

    void simulate(Progressor& progressor, Count const time_step);

    void finalize(Progressor& progressor);

    void postprocess(Progressor& progressor);

}  // namespace lue
