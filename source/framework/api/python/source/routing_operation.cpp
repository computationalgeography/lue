#include "lue/framework/api/cxx/routing_operation.hpp"
#include <pybind11/pybind11.h>


namespace lue::api {

    void bind_routing_operations(pybind11::module& module)
    {
        module.def("accu", accu);
    }

}  // namespace lue::api
