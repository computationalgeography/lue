#include "lue/framework/model/simulate.hpp"
#include "lue/framework/model/model.hpp"
#include "lue/framework/model/progressor.hpp"
#include <pybind11/pybind11.h>


using namespace pybind11::literals;


namespace lue::framework {

    void bind_simulate(pybind11::module& module)
    {
        module.def(
            "run_deterministic",
            run_deterministic<Model, Progressor>,
            "model"_a,
            "progressor"_a,
            "nr_time_steps"_a,
            "rate_limit"_a = 0);
        module.def(
            "run_stochastic",
            run_stochastic<Model, Progressor>,
            "model"_a,
            "progressor"_a,
            "nr_samples"_a,
            "nr_time_steps"_a,
            "rate_limit"_a = 0);
    }

}  // namespace lue::framework
