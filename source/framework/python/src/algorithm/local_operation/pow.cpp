#include "lue/py/framework/algorithm/pow.hpp"
#include <pybind11/pybind11.h>


namespace lue::framework {
    namespace {

        template<typename Element>
        constexpr void define_pow_overloads(pybind11::module& module)
        {
            Rank const rank{2};

            module.def(
                "pow", pow<Element, rank, PartitionedArray<Element, rank>, PartitionedArray<Element, rank>>);
            module.def("pow", pow<Element, rank, PartitionedArray<Element, rank>, Scalar<Element>>);
            module.def("pow", pow<Element, rank, PartitionedArray<Element, rank>, Element>);

            module.def("pow", pow<Element, rank, Scalar<Element>, PartitionedArray<Element, rank>>);
            module.def("pow", pow<Element, rank, Element, PartitionedArray<Element, rank>>);

            module.def("pow", pow<Element, Scalar<Element>, Element>);
            module.def("pow", pow<Element, Element, Scalar<Element>>);
        }

    }  // Anonymous namespace


    void bind_pow(pybind11::module& module)
    {
        define_pow_overloads<float>(module);
        define_pow_overloads<double>(module);
    }

}  // namespace lue::framework
