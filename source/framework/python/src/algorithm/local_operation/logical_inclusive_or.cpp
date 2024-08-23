#include "lue/framework/algorithm/value_policies/logical_inclusive_or.hpp"
#include <pybind11/pybind11.h>


namespace lue::framework {
    namespace {

        template<typename Element>
        constexpr void define_logical_inclusive_or_overloads(pybind11::module& module)
        {
            using namespace value_policies;
            Rank const rank{2};
            using Array = PartitionedArray<Element, rank>;
            using Scalar = Scalar<Element>;
            using Value = Element;
            using BooleanElement = std::uint8_t;

            module.def(
                "logical_inclusive_or",
                [](Array const& array1, Array const& array2)
                { return logical_inclusive_or<BooleanElement>(array1, array2); });
            module.def(
                "logical_inclusive_or",
                [](Array const& array, Scalar const& scalar)
                { return logical_inclusive_or<BooleanElement>(array, scalar); });
            module.def(
                "logical_inclusive_or",
                [](Array const& array, Value const value)
                { return logical_inclusive_or<BooleanElement>(array, value); });

            module.def(
                "logical_inclusive_or",
                [](Scalar const& scalar, Array const& array)
                { return logical_inclusive_or<BooleanElement>(scalar, array); });
            module.def(
                "logical_inclusive_or",
                [](Scalar const& scalar1, Scalar const& scalar2)
                { return logical_inclusive_or<BooleanElement>(scalar1, scalar2); });
            module.def(
                "logical_inclusive_or",
                [](Scalar const& scalar, Value const value)
                { return logical_inclusive_or<BooleanElement>(scalar, value); });

            module.def(
                "logical_inclusive_or",
                [](Value const value, Array const& array)
                { return logical_inclusive_or<BooleanElement>(value, array); });
            module.def(
                "logical_inclusive_or",
                [](Value const value, Scalar const& scalar)
                { return logical_inclusive_or<BooleanElement>(value, scalar); });

            module.def(
                "logical_inclusive_or",
                [](Value const value1, Value const value2)
                { return logical_inclusive_or<BooleanElement>(value1, value2); });
        }

    }  // Anonymous namespace


    void bind_logical_inclusive_or(pybind11::module& module)
    {
        define_logical_inclusive_or_overloads<std::uint8_t>(module);
        define_logical_inclusive_or_overloads<std::uint32_t>(module);
        define_logical_inclusive_or_overloads<std::uint64_t>(module);
        define_logical_inclusive_or_overloads<std::int32_t>(module);
        define_logical_inclusive_or_overloads<std::int64_t>(module);
    }

}  // namespace lue::framework
