#include "lue/py/framework/algorithm/pow.hpp"
#include <pybind11/pybind11.h>


namespace lue::framework {

#define LUE_POW_OVERLOADS(type, rank)                                                                        \
    module.def("pow", pow<type, rank, PartitionedArray<type, rank>, PartitionedArray<type, rank>>);          \
    module.def("pow", pow<type, rank, PartitionedArray<type, rank>, type>);                                  \
    module.def("pow", pow<type, rank, PartitionedArray<type, rank>, hpx::shared_future<type>>);              \
    module.def("pow", pow<type, rank, type, PartitionedArray<type, rank>>);                                  \
    module.def("pow", pow<type, rank, hpx::shared_future<type>, PartitionedArray<type, rank>>);


    void bind_pow(pybind11::module& module)
    {
        // TODO How to document these?
        LUE_POW_OVERLOADS(float, 2)
        LUE_POW_OVERLOADS(double, 2)
    }

}  // namespace lue::framework
