#include "lue/framework/algorithm/value_policies/less_than.hpp"
#include <pybind11/pybind11.h>


namespace lue::framework {
    namespace {

        template<typename Element>
        constexpr void define_less_than_overloads(pybind11::module& module)
        {
            using namespace value_policies;
            Rank const rank{2};
            using Array = PartitionedArray<Element, rank>;
            using Scalar = Scalar<Element>;
            using Value = Element;
            using BooleanElement = std::uint8_t;

            module.def(
                "less_than",
                [](Array const& array1, Array const& array2)
                { return less_than<BooleanElement>(array1, array2); });
            module.def(
                "less_than",
                [](Array const& array, Scalar const& scalar)
                { return less_than<BooleanElement>(array, scalar); });
            module.def(
                "less_than",
                [](Array const& array, Value const value)
                { return less_than<BooleanElement>(array, value); });

            module.def(
                "less_than",
                [](Scalar const& scalar, Array const& array)
                { return less_than<BooleanElement>(scalar, array); });
            module.def(
                "less_than",
                [](Scalar const& scalar1, Scalar const& scalar2)
                { return less_than<BooleanElement>(scalar1, scalar2); });
            module.def(
                "less_than",
                [](Scalar const& scalar, Value const value)
                { return less_than<BooleanElement>(scalar, value); });

            module.def(
                "less_than",
                [](Value const value, Array const& array)
                { return less_than<BooleanElement>(value, array); });
            module.def(
                "less_than",
                [](Value const value, Scalar const& scalar)
                { return less_than<BooleanElement>(value, scalar); });

            module.def(
                "less_than",
                [](Value const value1, Value const value2)
                { return less_than<BooleanElement>(value1, value2); });
        }

    }  // Anonymous namespace


    void bind_less_than(pybind11::module& module)
    {
        define_less_than_overloads<std::uint8_t>(module);
        define_less_than_overloads<std::uint32_t>(module);
        define_less_than_overloads<std::uint64_t>(module);
        define_less_than_overloads<std::int32_t>(module);
        define_less_than_overloads<std::int64_t>(module);
        define_less_than_overloads<float>(module);
        define_less_than_overloads<double>(module);
    }

}  // namespace lue::framework
