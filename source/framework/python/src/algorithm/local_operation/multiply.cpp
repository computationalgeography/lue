#include "lue/py/framework/algorithm/multiply.hpp"
#include <pybind11/pybind11.h>


namespace lue::framework {
    namespace {

        template<typename Element>
        constexpr void define_multiply_overloads(pybind11::module& module)
        {
            Rank const rank{2};

            module.def(
                "multiply",
                multiply<Element, rank, PartitionedArray<Element, rank>, PartitionedArray<Element, rank>>);
            module.def("multiply", multiply<Element, rank, PartitionedArray<Element, rank>, Scalar<Element>>);
            module.def("multiply", multiply<Element, rank, PartitionedArray<Element, rank>, Element>);

            module.def("multiply", multiply<Element, rank, Scalar<Element>, PartitionedArray<Element, rank>>);
            module.def("multiply", multiply<Element, rank, Element, PartitionedArray<Element, rank>>);

            module.def("multiply", multiply<Element, Scalar<Element>, Element>);
            module.def("multiply", multiply<Element, Element, Scalar<Element>>);
        }

    }  // Anonymous namespace


    void bind_multiply(pybind11::module& module)
    {
        define_multiply_overloads<float>(module);
        define_multiply_overloads<double>(module);
    }

}  // namespace lue::framework
