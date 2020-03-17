#pragma once


namespace lue {

/*!
    @brief      Simulate a process by running a model
    @param      model Model to use
    @param      nr_time_steps Number of time-steps to simulate

    This function template is implemented in terms of customization
    points. They are called in order:

    - preprocess()
    - initialize()
    - simulate(), for each time step
    - postprocess()

    These customization points must be implemented for the @a Model type
    of the model passed in.
*/
template<
    typename Model>
void simulate(
    Model& model,
    Count const nr_time_steps)
{
    preprocess(model);
    initialize(model);

    for(Count t = 0; t < nr_time_steps; ++t) {
        simulate(model, t);
    }

    postprocess(model);
}

}  // namespace lue
