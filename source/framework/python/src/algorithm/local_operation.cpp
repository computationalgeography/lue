#include <pybind11/pybind11.h>


namespace lue::framework {

    void bind_add(pybind11::module& module);
    void bind_all(pybind11::module& module);
    void bind_divide(pybind11::module& module);
    void bind_equal_to(pybind11::module& module);
    void bind_maximum(pybind11::module& module);
    void bind_minimum(pybind11::module& module);
    void bind_multiply(pybind11::module& module);
    void bind_sqrt(pybind11::module& module);
    void bind_subtract(pybind11::module& module);
    void bind_uniform(pybind11::module& module);


    void bind_local_operations(
        pybind11::module& module)
    {
        bind_add(module);
        bind_all(module);
        bind_divide(module);
        bind_equal_to(module);
        bind_maximum(module);
        bind_minimum(module);
        bind_sqrt(module);
        bind_subtract(module);
        bind_multiply(module);
        bind_uniform(module);
    }

}  // namespace lue::framework
