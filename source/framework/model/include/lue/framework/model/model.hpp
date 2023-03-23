#pragma once
#include "lue/framework/core/define.hpp"
#include <hpx/future.hpp>


namespace lue {

    /*!
        @brief      Base class for model simulating some process

        New models can specialize this class. This is convenient because
        then the default customization points used by the @a lue::simulate()
        function template can be used.
    */
    class Model
    {

        public:

            Model() = default;

            Model(Model const&) = default;

            Model(Model&&) = default;

            virtual ~Model() = default;

            Model& operator=(Model const&) = default;

            Model& operator=(Model&&) = default;

            virtual void preprocess(Count const sample_nr);

            virtual void initialize();

            virtual hpx::shared_future<void> simulate(Count const time_step);

            virtual void finalize();

            virtual void postprocess();

        private:
    };


    void preprocess(Model& model, Count const sample_nr);

    void initialize(Model& model);

    hpx::shared_future<void> simulate(Model& model, Count const time_step);

    void finalize(Model& model);

    void postprocess(Model& model);

}  // namespace lue
