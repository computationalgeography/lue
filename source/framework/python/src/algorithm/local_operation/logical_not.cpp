#include "lue/framework/algorithm/value_policies/logical_not.hpp"
#include <pybind11/pybind11.h>


namespace lue::framework {
    namespace {

        template<typename Element>
        constexpr void define_logical_not_overloads(pybind11::module& module)
        {
            using namespace lue::value_policies;
            Rank const rank{2};
            using Array = PartitionedArray<Element, rank>;
            using Scalar = Scalar<Element>;
            using Value = Element;
            using BooleanElement = std::uint8_t;

            module.def("logical_not", [](Array const& array) { return logical_not<BooleanElement>(array); });
            module.def(
                "logical_not", [](Scalar const& scalar) { return logical_not<BooleanElement>(scalar); });
            module.def("logical_not", [](Value const value) { return logical_not<BooleanElement>(value); });
        }

    }  // Anonymous namespace


    void bind_logical_not(pybind11::module& module)
    {
        define_logical_not_overloads<std::uint8_t>(module);
        define_logical_not_overloads<std::int32_t>(module);
        define_logical_not_overloads<std::uint32_t>(module);
        define_logical_not_overloads<std::int64_t>(module);
        define_logical_not_overloads<std::uint64_t>(module);
    }

}  // namespace lue::framework
