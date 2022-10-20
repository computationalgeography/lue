#include "lue/py/framework/algorithm/logical_exclusive_or.hpp"
#include <pybind11/pybind11.h>


namespace lue::framework {

    void bind_logical_exclusive_or(
        pybind11::module& module)
    {
        module.def("logical_exclusive_or", logical_exclusive_or<std::uint8_t, 2>);
        module.def("logical_exclusive_or", logical_exclusive_or<std::int32_t, 2>);
        module.def("logical_exclusive_or", logical_exclusive_or<std::uint32_t, 2>);
        module.def("logical_exclusive_or", logical_exclusive_or<std::int64_t, 2>);
        module.def("logical_exclusive_or", logical_exclusive_or<std::uint64_t, 2>);
    }

}  // namespace lue::framework
