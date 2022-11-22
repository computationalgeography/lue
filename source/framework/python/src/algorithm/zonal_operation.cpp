#include <pybind11/pybind11.h>


namespace lue::framework {

    void bind_zonal_area(pybind11::module& module);
    void bind_zonal_maximum(pybind11::module& module);
    void bind_zonal_minimum(pybind11::module& module);
    void bind_zonal_sum(pybind11::module& module);


    void bind_zonal_operations(
        pybind11::module& module)
    {
        bind_zonal_area(module);
        bind_zonal_maximum(module);
        bind_zonal_minimum(module);
        bind_zonal_sum(module);
    }

}  // namespace lue::framework
