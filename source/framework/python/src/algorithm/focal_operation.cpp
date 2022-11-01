#include <pybind11/pybind11.h>


namespace lue::framework {

    void bind_focal_diversity(pybind11::module& module);
    void bind_focal_high_pass(pybind11::module& module);
    void bind_focal_majority(pybind11::module& module);
    void bind_focal_maximum(pybind11::module& module);
    void bind_focal_mean(pybind11::module& module);
    void bind_focal_minimum(pybind11::module& module);
    void bind_focal_sum(pybind11::module& module);


    void bind_focal_operations(
        pybind11::module& module)
    {
        bind_focal_diversity(module);
        bind_focal_high_pass(module);
        bind_focal_majority(module);
        bind_focal_maximum(module);
        bind_focal_mean(module);
        bind_focal_minimum(module);
        bind_focal_sum(module);
    }

}  // namespace lue::framework
