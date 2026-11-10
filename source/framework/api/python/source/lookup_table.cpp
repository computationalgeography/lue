#include "lue/framework/api/cxx/lookup_table.hpp"
#include <pybind11/pybind11.h>


namespace lue::api {

    void bind_lookup_table(pybind11::module& module)
    {
        pybind11::class_<LookupTable>(module, "LookupTable");
    }

}  // namespace lue::api
