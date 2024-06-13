#include <pybind11/pybind11.h>


namespace lue::framework {

    void bind_reclassify(pybind11::module& module);
    void bind_maximum(pybind11::module& module);
    void bind_minimum(pybind11::module& module);
    void bind_sum(pybind11::module& module);


    void bind_global_operations(pybind11::module& module)
    {
        bind_reclassify(module);
        bind_maximum(module);
        bind_minimum(module);
        bind_sum(module);
    }

}  // namespace lue::framework
