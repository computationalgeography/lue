#include "lue/framework/api/cxx/focal_operation.hpp"
#include <pybind11/stl.h>


namespace lue::api {

    void bind_focal_operations(pybind11::module& module)
    {
        module.def("focal_mean", focal_mean);
    }

}  // namespace lue::api
