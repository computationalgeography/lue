#include "lue/framework/api/cxx/kernel.hpp"
#include <pybind11/pybind11.h>


namespace lue::api {

    void bind_kernel(pybind11::module& module)
    {
        pybind11::class_<Kernel>(module, "Kernel");
    }

}  // namespace lue::api
