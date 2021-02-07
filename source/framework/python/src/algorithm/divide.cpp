#include "lue/py/framework/algorithm/divide.hpp"
#include <pybind11/pybind11.h>


namespace lue::framework {

#define LUE_DIVIDE_OVERLOADS(type, rank) \
    module.def("divide", divide<type, rank, PartitionedArray<type, rank>, PartitionedArray<type, rank>>);  \
    module.def("divide", divide<type, rank, PartitionedArray<type, rank>, type>                        );  \
    module.def("divide", divide<type, rank, PartitionedArray<type, rank>, hpx::shared_future<type>>    );  \
    module.def("divide", divide<type, rank, type                        , PartitionedArray<type, rank>>);  \
    module.def("divide", divide<type, rank, hpx::shared_future<type>    , PartitionedArray<type, rank>>);


    void bind_divide(
        pybind11::module& module)
    {
        // TODO How to document these?
        // LUE_DIVIDE_OVERLOADS(std::uint8_t, 2)
        // LUE_DIVIDE_OVERLOADS(std::uint32_t, 2)
        // LUE_DIVIDE_OVERLOADS(std::uint64_t, 2)
        // LUE_DIVIDE_OVERLOADS(std::int32_t, 2)
        // LUE_DIVIDE_OVERLOADS(std::int64_t, 2)
        LUE_DIVIDE_OVERLOADS(float, 2)
        LUE_DIVIDE_OVERLOADS(double, 2)
    }

}  // namespace lue::framework
