#include "lue/framework/model/model.hpp"


namespace lue {

    /*!
        @brief      Do whatever it takes to prepare the model for initialization

        The default does nothing.
    */
    void Model::preprocess([[maybe_unused]] Count const sample_nr)
    {
    }


    /*!
        @brief      Initialize the modelled state

        The current state of the simulated system is undefined. After calling this function, it must be
        representable for t = 0.

        The default does nothing.
    */
    void Model::initialize()
    {
    }


    /*!
        @brief      Simulate the state of the modelled system during an additional time step (t = t + 1)
        @warning    The returned future must reflect the status of the work that has to be done to end up
                    state of t + 1. The future must become ready only once this work has finished. Typically,
                    the (partitions of the) last state variable can be used for that. Knowing when the work
                    related to a time step has finished is crucial for being able to limit the rate with
                    which tasks are created (see for example run_deterministic()).

        The current state of the simulated system is representable for t = t. After calling this function, it
        must be representable for t = t + 1.

        The default does nothing.
    */
    auto Model::simulate([[maybe_unused]] Count const time_step) -> hpx::shared_future<void>
    {
        return hpx::make_ready_future();
    }


    /*!
        @brief      Terminate the model

        The default does nothing.
    */
    void Model::finalize()
    {
    }


    /*!
        @brief      Postprocess the model results

        This function is called after the simulation has finished.

        The default does nothing.
    */
    void Model::postprocess()
    {
    }


    /*!
        @brief      Call Model.preprocess()
    */
    void preprocess(Model& model, Count const sample_nr)
    {
        model.preprocess(sample_nr);
    }


    /*!
        @brief      Call Model.initialize()
    */
    void initialize(Model& model)
    {
        model.initialize();
    }


    /*!
        @brief      Call Model.simulate(Count const)
    */
    auto simulate(Model& model, Count const time_step) -> hpx::shared_future<void>
    {
        return model.simulate(time_step);
    }


    /*!
        @brief      Call Model.finalize()
    */
    void finalize(Model& model)
    {
        model.finalize();
    }


    /*!
        @brief      Call Model.postprocess()
    */
    void postprocess(Model& model)
    {
        model.postprocess();
    }

}  // namespace lue
