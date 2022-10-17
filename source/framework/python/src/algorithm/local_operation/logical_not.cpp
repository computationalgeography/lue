#include "lue/framework/algorithm/value_policies/logical_not.hpp"
#include <pybind11/pybind11.h>


namespace lue::framework {

    void bind_logical_not(
        pybind11::module& module)
    {
        module.def("logical_not", value_policies::logical_not<std::uint8_t, 2>);
        module.def("logical_not", value_policies::logical_not<std::int32_t, 2>);
        module.def("logical_not", value_policies::logical_not<std::uint32_t, 2>);
        module.def("logical_not", value_policies::logical_not<std::int64_t, 2>);
        module.def("logical_not", value_policies::logical_not<std::uint64_t, 2>);
    }

}  // namespace lue::framework
