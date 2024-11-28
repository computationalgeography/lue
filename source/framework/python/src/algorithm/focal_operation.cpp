#include <pybind11/pybind11.h>


namespace lue::framework {

    void bind_focal(pybind11::module& module);


    PYBIND11_EXPORT void bind_focal_operations(pybind11::module& module)
    {
        bind_focal(module);
    }

}  // namespace lue::framework
