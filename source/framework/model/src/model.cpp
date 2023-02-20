#include "lue/framework/model/model.hpp"


namespace lue {

    /*!
        @brief      Do whatever it takes to prepare the model for initialization

        The default does nothing.
    */
    void Model::preprocess()
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
    void Model::simulate(Count const /* time_step */)
    {
    }


    /*!
        @brief      Terminate the model

        The default does nothing.
    */
    void Model::terminate()
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
    void preprocess(Model& model)
    {
        model.preprocess();
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
    void simulate(Model& model, Count const time_step)
    {
        model.simulate(time_step);
    }


    /*!
        @brief      Call Model.terminate()
    */
    void terminate(Model& model)
    {
        model.terminate();
    }


    /*!
        @brief      Call Model.postprocess()
    */
    void postprocess(Model& model)
    {
        model.postprocess();
    }

}  // namespace lue
