#pragma once
#include "lue/framework/core/define.hpp"
#include "lue/framework/model/export.hpp"
#include <hpx/future.hpp>


namespace lue {

    /*!
        @brief      Base class for model simulating some process

        New models can specialize this class. This is convenient because
        then the default customization points used by the @a lue::simulate()
        function template can be used.
    */
    class LUE_FRAMEWORK_MODEL_EXPORT Model
    {

        public:

            Model() = default;

            Model(Model const&) = default;

            Model(Model&&) = default;

            virtual ~Model() = default;

            auto operator=(Model const&) -> Model& = default;

            auto operator=(Model&&) -> Model& = default;

            virtual void preprocess(Count sample_nr);

            virtual void initialize();

            virtual auto simulate(Count time_step) -> hpx::shared_future<void>;

            virtual void finalize();

            virtual void postprocess();

        private:
    };


    LUE_FRAMEWORK_MODEL_EXPORT void preprocess(Model& model, Count sample_nr);

    LUE_FRAMEWORK_MODEL_EXPORT void initialize(Model& model);

    LUE_FRAMEWORK_MODEL_EXPORT auto simulate(Model& model, Count time_step) -> hpx::shared_future<void>;

    LUE_FRAMEWORK_MODEL_EXPORT void finalize(Model& model);

    LUE_FRAMEWORK_MODEL_EXPORT void postprocess(Model& model);

}  // namespace lue
