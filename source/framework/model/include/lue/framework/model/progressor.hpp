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

            auto operator=(Progressor const&) -> Progressor& = default;

            auto operator=(Progressor&&) -> Progressor& = default;

            virtual void preprocess(Count sample_nr);

            virtual void initialize();

            virtual void simulate(Count time_step);

            virtual void finalize();

            virtual void postprocess();

        private:
    };


    void preprocess(Progressor& progressor, Count sample_nr);

    void initialize(Progressor& progressor);

    void simulate(Progressor& progressor, Count time_step);

    void finalize(Progressor& progressor);

    void postprocess(Progressor& progressor);

}  // namespace lue
