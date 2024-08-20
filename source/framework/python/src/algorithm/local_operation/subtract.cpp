#include "lue/py/framework/algorithm/subtract.hpp"
#include <pybind11/pybind11.h>


namespace lue::framework {
    namespace {

        template<typename Element>
        constexpr void define_subtract_overloads(pybind11::module& module)
        {
            Rank const rank{2};

            module.def(
                "subtract",
                subtract<Element, rank, PartitionedArray<Element, rank>, PartitionedArray<Element, rank>>);
            module.def("subtract", subtract<Element, rank, PartitionedArray<Element, rank>, Scalar<Element>>);
            module.def("subtract", subtract<Element, rank, PartitionedArray<Element, rank>, Element>);

            module.def("subtract", subtract<Element, rank, Scalar<Element>, PartitionedArray<Element, rank>>);
            module.def("subtract", subtract<Element, rank, Element, PartitionedArray<Element, rank>>);

            module.def("subtract", subtract<Element, Scalar<Element>, Element>);
            module.def("subtract", subtract<Element, Element, Scalar<Element>>);
        }

    }  // Anonymous namespace


    void bind_subtract(pybind11::module& module)
    {
        define_subtract_overloads<std::uint8_t>(module);
        define_subtract_overloads<std::uint32_t>(module);
        define_subtract_overloads<std::uint64_t>(module);
        define_subtract_overloads<std::int32_t>(module);
        define_subtract_overloads<std::int64_t>(module);
        define_subtract_overloads<float>(module);
        define_subtract_overloads<double>(module);
    }

}  // namespace lue::framework
