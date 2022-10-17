#include "lue/py/framework/algorithm/logical_and.hpp"
#include <pybind11/pybind11.h>


namespace lue::framework {

    void bind_logical_and(
        pybind11::module& module)
    {
        module.def("logical_and", logical_and<std::uint8_t, 2>);
        module.def("logical_and", logical_and<std::int32_t, 2>);
        module.def("logical_and", logical_and<std::uint32_t, 2>);
        module.def("logical_and", logical_and<std::int64_t, 2>);
        module.def("logical_and", logical_and<std::uint64_t, 2>);
    }

}  // namespace lue::framework
