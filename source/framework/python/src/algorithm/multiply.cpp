#include "lue/py/framework/algorithm/multiply.hpp"
#include <pybind11/pybind11.h>


namespace lue::framework {

#define LUE_MULTIPLY_OVERLOADS(type, rank)                                                                   \
    module.def(                                                                                              \
        "multiply", multiply<type, rank, PartitionedArray<type, rank>, PartitionedArray<type, rank>>);       \
    module.def("multiply", multiply<type, rank, PartitionedArray<type, rank>, type>);                        \
    module.def("multiply", multiply<type, rank, PartitionedArray<type, rank>, hpx::shared_future<type>>);    \
    module.def("multiply", multiply<type, rank, type, PartitionedArray<type, rank>>);                        \
    module.def("multiply", multiply<type, rank, hpx::shared_future<type>, PartitionedArray<type, rank>>);


    void bind_multiply(pybind11::module& module)
    {
        // TODO How to document these?
        LUE_MULTIPLY_OVERLOADS(float, 2)
        LUE_MULTIPLY_OVERLOADS(double, 2)
    }

}  // namespace lue::framework
