#include "lue/py/framework/algorithm/divide.hpp"
#include <pybind11/pybind11.h>


namespace lue::framework {
    namespace {

        template<typename Element>
        constexpr void define_divide_overloads(pybind11::module& module)
        {
            Rank const rank{2};

            module.def(
                "divide",
                divide<Element, rank, PartitionedArray<Element, rank>, PartitionedArray<Element, rank>>);
            module.def("divide", divide<Element, rank, PartitionedArray<Element, rank>, Scalar<Element>>);
            module.def("divide", divide<Element, rank, PartitionedArray<Element, rank>, Element>);

            module.def("divide", divide<Element, rank, Scalar<Element>, PartitionedArray<Element, rank>>);
            module.def("divide", divide<Element, rank, Element, PartitionedArray<Element, rank>>);

            module.def("divide", divide<Element, Scalar<Element>, Element>);
            module.def("divide", divide<Element, Element, Scalar<Element>>);
        }

    }  // Anonymous namespace


    void bind_divide(pybind11::module& module)
    {
        define_divide_overloads<float>(module);
        define_divide_overloads<double>(module);
    }

}  // namespace lue::framework
