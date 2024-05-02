#include "lue/framework/api/cxx/local_operation.hpp"
#include <pybind11/pybind11.h>


namespace lue {

    void bind_local_operations(pybind11::module& module)
    {
        module.def("add", add);
        module.def("uniform", uniform);
    }

}  // namespace lue
