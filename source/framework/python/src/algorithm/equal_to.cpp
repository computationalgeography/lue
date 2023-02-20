#include "lue/py/framework/algorithm/equal_to.hpp"
#include "lue/py/framework/type_traits.hpp"
#include <pybind11/pybind11.h>


#define LUE_EQUAL_TO_OVERLOADS(type, rank)                                                                   \
    module.def(                                                                                              \
        "equal_to",                                                                                          \
        equal_to<type, std::uint8_t, rank, PartitionedArray<type, rank>, PartitionedArray<type, rank>>);     \
    module.def("equal_to", equal_to<type, std::uint8_t, rank, PartitionedArray<type, rank>, type>);          \
    module.def(                                                                                              \
        "equal_to",                                                                                          \
        equal_to<type, std::uint8_t, rank, PartitionedArray<type, rank>, hpx::shared_future<type>>);         \
    module.def("equal_to", equal_to<type, std::uint8_t, rank, type, PartitionedArray<type, rank>>);          \
    module.def(                                                                                              \
        "equal_to",                                                                                          \
        equal_to<type, std::uint8_t, rank, hpx::shared_future<type>, PartitionedArray<type, rank>>);


namespace lue::framework {

    void bind_equal_to(pybind11::module& module)
    {
        LUE_EQUAL_TO_OVERLOADS(std::uint8_t, 2)
        LUE_EQUAL_TO_OVERLOADS(std::uint32_t, 2)
        LUE_EQUAL_TO_OVERLOADS(std::uint64_t, 2)
        LUE_EQUAL_TO_OVERLOADS(std::int32_t, 2)
        LUE_EQUAL_TO_OVERLOADS(std::int64_t, 2)
        LUE_EQUAL_TO_OVERLOADS(float, 2)
        LUE_EQUAL_TO_OVERLOADS(double, 2)
    }

}  // namespace lue::framework
