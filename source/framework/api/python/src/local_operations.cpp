#include "lue/framework/api/cxx/local/add.hpp"
#include <pybind11/pybind11.h>


namespace lue {

    void bind_local_operations(pybind11::module& module)
    {
        module.def("add", add);
    }

}  // namespace lue
