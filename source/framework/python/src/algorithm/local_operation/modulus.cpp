#include "lue/py/framework/algorithm/modulus.hpp"
#include <pybind11/pybind11.h>

namespace lue::framework {

#define LUE_MODULUS_OVERLOADS(type, rank)                                                                    \
    module.def("modulus", modulus<type, rank, PartitionedArray<type, rank>, PartitionedArray<type, rank>>);  \
    module.def("modulus", modulus<type, rank, PartitionedArray<type, rank>, type>);                          \
    module.def("modulus", modulus<type, rank, PartitionedArray<type, rank>, hpx::shared_future<type>>);      \
    module.def("modulus", modulus<type, rank, type, PartitionedArray<type, rank>>);                          \
    module.def("modulus", modulus<type, rank, hpx::shared_future<type>, PartitionedArray<type, rank>>);


    void bind_modulus(pybind11::module& module)
    {
        LUE_MODULUS_OVERLOADS(int32_t, 2)
        LUE_MODULUS_OVERLOADS(int64_t, 2)
    }

}  // namespace lue::framework
