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
    @brief      Initialize the modelled state.

    After calling this function, the simulated state must be representable
    for t = 0.

    The default does nothing.
*/
void Model::initialize()
{
}


/*!
    @brief      Simulate the state of the modelled system during an
                additional time step (t = t + 1)

    After calling this function, the simulated state must be representable
    for t = t + 1.

    The default does nothing.
*/
void Model::simulate()
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
void preprocess(
    Model& model)
{
    model.preprocess();
}


/*!
    @brief      Call Model.initialize()
*/
void initialize(
    Model& model)
{
    model.initialize();
}


/*!
    @brief      Call Model.simulate()
*/
void simulate(
    Model& model,
    Count const /* time_step */)
{
    model.simulate();
}


/*!
    @brief      Call Model.postprocess()
*/
void postprocess(
    Model& model)
{
    model.postprocess();
}

}  // namespace lue
