#include <pybind11/pybind11.h>


namespace lue::framework {

    void bind_resample(pybind11::module& module);


    PYBIND11_EXPORT void bind_miscellaneous_operations(pybind11::module& module)
    {
        bind_resample(module);
    }

}  // namespace lue::framework
