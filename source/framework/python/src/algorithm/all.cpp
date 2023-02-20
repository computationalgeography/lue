#include "lue/py/framework/algorithm/all.hpp"
#include <pybind11/pybind11.h>


namespace lue::framework {

#define LUE_ALL_OVERLOADS(type, rank) module.def("all", all<type, rank>);


    void bind_all(pybind11::module& module)
    {
        // TODO How to document these?
        LUE_ALL_OVERLOADS(std::uint8_t, 2)
        LUE_ALL_OVERLOADS(std::uint32_t, 2)
        LUE_ALL_OVERLOADS(std::uint64_t, 2)
        LUE_ALL_OVERLOADS(std::int32_t, 2)
        LUE_ALL_OVERLOADS(std::int64_t, 2)
        LUE_ALL_OVERLOADS(float, 2)
        LUE_ALL_OVERLOADS(double, 2)
    }

}  // namespace lue::framework
