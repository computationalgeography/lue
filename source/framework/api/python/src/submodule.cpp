#include "lue/py/framework_x/submodule.hpp"
#include <pybind11/pybind11.h>


namespace lue::api {

    void bind_local_operations(pybind11::module& module);
    void bind_array(pybind11::module& module);
    void bind_field(pybind11::module& module);
    void bind_io(pybind11::module& module);
    void bind_scalar(pybind11::module& module);


    void init_framework_x(pybind11::module& module)
    {
        pybind11::module submodule = module.def_submodule("framework_x");

        bind_array(submodule);
        bind_field(submodule);
        bind_scalar(submodule);

        bind_io(submodule);
        bind_local_operations(submodule);
    }

}  // namespace lue::api
