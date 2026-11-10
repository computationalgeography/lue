#include "lue/py/framework_x/submodule.hpp"
#include "hpx_runtime.hpp"
#include <pybind11/pybind11.h>


namespace lue::api {

    void bind_array(pybind11::module& module);
    void bind_as_field(pybind11::module& module);
    void bind_as_state(pybind11::module& module);
    void bind_field(pybind11::module& module);
    void bind_focal_operations(pybind11::module& module);
    void bind_io(pybind11::module& module);
    void bind_kernel(pybind11::module& module);
    void bind_literal(pybind11::module& module);
    void bind_lookup_table(pybind11::module& module);
    void bind_local_operations(pybind11::module& module);
    void bind_miscellaneous_operations(pybind11::module& module);
    void bind_routing_operations(pybind11::module& module);
    void bind_scalar(pybind11::module& module);
    void bind_type_info(pybind11::module& module);


    void init_framework_x(pybind11::module& module)
    {
        pybind11::module submodule = module.def_submodule("framework_x");

        bind_array(submodule);
        bind_field(submodule);
        bind_kernel(submodule);
        bind_literal(submodule);
        bind_lookup_table(submodule);
        bind_scalar(submodule);
        bind_type_info(submodule);

        bind_as_field(submodule);
        bind_as_state(submodule);

        bind_focal_operations(submodule);
        bind_io(submodule);
        bind_local_operations(submodule);
        bind_miscellaneous_operations(submodule);
        bind_routing_operations(submodule);
    }

}  // namespace lue::api
