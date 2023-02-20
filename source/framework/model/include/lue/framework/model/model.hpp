#pragma once
#include "lue/framework/core/define.hpp"


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

            virtual void preprocess();

            virtual void initialize();

            virtual void simulate(Count time_step);

            virtual void terminate();

            virtual void postprocess();

        private:
    };


    void preprocess(Model& model);

    void initialize(Model& model);

    void simulate(Model& model, Count time_step);

    void terminate(Model& model);

    void postprocess(Model& model);

}  // namespace lue
