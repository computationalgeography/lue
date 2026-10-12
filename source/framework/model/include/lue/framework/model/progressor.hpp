#pragma once
#include "lue/framework/core/define.hpp"
#include "lue/framework/model/export.hpp"


namespace lue {

    /*!
        @brief      Base class for progressors

        When a model is executed, progressors can be used to visualize the progress.
    */
    class LUE_FRAMEWORK_MODEL_EXPORT Progressor
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


    LUE_FRAMEWORK_MODEL_EXPORT void preprocess(Progressor& progressor, Count sample_nr);

    LUE_FRAMEWORK_MODEL_EXPORT void initialize(Progressor& progressor);

    LUE_FRAMEWORK_MODEL_EXPORT void simulate(Progressor& progressor, Count time_step);

    LUE_FRAMEWORK_MODEL_EXPORT void finalize(Progressor& progressor);

    LUE_FRAMEWORK_MODEL_EXPORT void postprocess(Progressor& progressor);

}  // namespace lue
