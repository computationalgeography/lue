#pragma once
#include "lue/framework/core/define.hpp"
#include <hpx/distributed/iostream.hpp>


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
    - terminate()
    - postprocess()

    These customization points must be implemented for the @a Model type
    of the model passed in.

    - preprocess() / postprocess() are not part of the actual simulation
        model. They do what needs to be done before and after the model
        has run.
    - initialize() / terminate() do what needs to be done before and
        after the simulation through time, like initialize the state at
        the start of the simulation (initialize()) and waiting for all
        tasks to finish (terminate()).
*/
template<
    typename Model>
void simulate_process(
    Model& model,
    Count const nr_time_steps)
{
    hpx::cout << "[preprocess... " << hpx::flush;
    preprocess(model);
    hpx::cout << ']' << hpx::endl;

    hpx::cout << "[initialize... " << hpx::flush;
    initialize(model);
    hpx::cout << ']' << hpx::endl;

    hpx::cout << "[simulate" << hpx::flush;
    for(Count t = 0; t < nr_time_steps; ++t) {
        simulate(model, t);
        hpx::cout << '.' << hpx::flush;
    }
    hpx::cout << ']' << hpx::endl;

    hpx::cout << "[terminate... " << hpx::flush;
    terminate(model);
    hpx::cout << ']' << hpx::endl;

    hpx::cout << "[postprocess..." << hpx::flush;
    postprocess(model);
    hpx::cout << ']' << hpx::endl;
}

}  // namespace lue
