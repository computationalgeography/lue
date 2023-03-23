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

        The current state of the simulated system is undefined. After calling
        this function, it must be representable for t = 0.

        The default does nothing.
    */
    void Model::initialize()
    {
    }


    /*!
        @brief      Simulate the state of the modelled system during an
                    additional time step (t = t + 1)

        The current state of the simulated system is representable for t =
        t. After calling this function, it must be representable for t =
        t + 1.

        The default does nothing.
    */
    hpx::shared_future<void> Model::simulate([[maybe_unused]] Count const time_step)
    {
        return hpx::make_ready_future<void>();
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
    hpx::shared_future<void> simulate(Model& model, Count const time_step)
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
