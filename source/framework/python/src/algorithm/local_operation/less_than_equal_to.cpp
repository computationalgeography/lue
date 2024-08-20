#include "lue/py/framework/algorithm/less_than_equal_to.hpp"
#include "lue/py/framework/type_traits.hpp"
#include <pybind11/pybind11.h>


namespace lue::framework {
    namespace {

        template<typename Element>
        constexpr void define_less_than_equal_to_overloads(pybind11::module& module)
        {
            Rank const rank{2};

            using OutputElement = std::uint8_t;

            module.def(
                "less_than_equal_to",
                less_than_equal_to<
                    OutputElement,
                    rank,
                    PartitionedArray<Element, rank>,
                    PartitionedArray<Element, rank>>);
            module.def(
                "less_than_equal_to",
                less_than_equal_to<OutputElement, rank, PartitionedArray<Element, rank>, Scalar<Element>>);
            module.def(
                "less_than_equal_to",
                less_than_equal_to<OutputElement, rank, PartitionedArray<Element, rank>, Element>);

            module.def(
                "less_than_equal_to",
                less_than_equal_to<OutputElement, rank, Scalar<Element>, PartitionedArray<Element, rank>>);
            module.def(
                "less_than_equal_to",
                less_than_equal_to<OutputElement, rank, Element, PartitionedArray<Element, rank>>);

            module.def("less_than_equal_to", less_than_equal_to<OutputElement, Scalar<Element>, Element>);
            module.def("less_than_equal_to", less_than_equal_to<OutputElement, Element, Scalar<Element>>);
        }

    }  // Anonymous namespace


    void bind_less_than_equal_to(pybind11::module& module)
    {
        define_less_than_equal_to_overloads<std::uint8_t>(module);
        define_less_than_equal_to_overloads<std::uint32_t>(module);
        define_less_than_equal_to_overloads<std::uint64_t>(module);
        define_less_than_equal_to_overloads<std::int32_t>(module);
        define_less_than_equal_to_overloads<std::int64_t>(module);
        define_less_than_equal_to_overloads<float>(module);
        define_less_than_equal_to_overloads<double>(module);
    }

}  // namespace lue::framework
