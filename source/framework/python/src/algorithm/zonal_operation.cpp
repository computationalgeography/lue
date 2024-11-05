#include <pybind11/pybind11.h>


namespace lue::framework {

    void bind_zonal_operation(pybind11::module& module);


    PYBIND11_EXPORT void bind_zonal_operations(pybind11::module& module)
    {
        bind_zonal_operation(module);
    }

}  // namespace lue::framework
