#include "lue/framework/algorithm/value_policies/greater_than_equal_to.hpp"
#include <pybind11/pybind11.h>


namespace lue::framework {
    namespace {

        template<typename Element>
        constexpr void define_greater_than_equal_to_overloads(pybind11::module& module)
        {
            using namespace value_policies;
            Rank const rank{2};
            using Array = PartitionedArray<Element, rank>;
            using Scalar = Scalar<Element>;
            using Value = Element;
            using BooleanElement = std::uint8_t;

            module.def(
                "greater_than_equal_to",
                [](Array const& array1, Array const& array2)
                { return greater_than_equal_to<BooleanElement>(array1, array2); });
            module.def(
                "greater_than_equal_to",
                [](Array const& array, Scalar const& scalar)
                { return greater_than_equal_to<BooleanElement>(array, scalar); });
            module.def(
                "greater_than_equal_to",
                [](Array const& array, Value const value)
                { return greater_than_equal_to<BooleanElement>(array, value); });

            module.def(
                "greater_than_equal_to",
                [](Scalar const& scalar, Array const& array)
                { return greater_than_equal_to<BooleanElement>(scalar, array); });
            module.def(
                "greater_than_equal_to",
                [](Scalar const& scalar1, Scalar const& scalar2)
                { return greater_than_equal_to<BooleanElement>(scalar1, scalar2); });
            module.def(
                "greater_than_equal_to",
                [](Scalar const& scalar, Value const value)
                { return greater_than_equal_to<BooleanElement>(scalar, value); });

            module.def(
                "greater_than_equal_to",
                [](Value const value, Array const& array)
                { return greater_than_equal_to<BooleanElement>(value, array); });
            module.def(
                "greater_than_equal_to",
                [](Value const value, Scalar const& scalar)
                { return greater_than_equal_to<BooleanElement>(value, scalar); });

            module.def(
                "greater_than_equal_to",
                [](Value const value1, Value const value2)
                { return greater_than_equal_to<BooleanElement>(value1, value2); });
        }

    }  // Anonymous namespace


    void bind_greater_than_equal_to(pybind11::module& module)
    {
        define_greater_than_equal_to_overloads<std::uint8_t>(module);
        define_greater_than_equal_to_overloads<std::uint32_t>(module);
        define_greater_than_equal_to_overloads<std::uint64_t>(module);
        define_greater_than_equal_to_overloads<std::int32_t>(module);
        define_greater_than_equal_to_overloads<std::int64_t>(module);
        define_greater_than_equal_to_overloads<float>(module);
        define_greater_than_equal_to_overloads<double>(module);
    }

}  // namespace lue::framework
