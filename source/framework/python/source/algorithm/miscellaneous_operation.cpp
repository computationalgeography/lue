#include <pybind11/pybind11.h>


namespace lue::framework {

    void bind_normal(pybind11::module& module);
    void bind_resample(pybind11::module& module);
    void bind_uniform1(pybind11::module& module);
    void bind_uniform2(pybind11::module& module);


    PYBIND11_EXPORT void bind_miscellaneous_operations(pybind11::module& module)
    {
        bind_normal(module);
        bind_resample(module);
        bind_uniform1(module);
        bind_uniform2(module);
    }

}  // namespace lue::framework
