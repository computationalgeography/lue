#include "lue/framework/algorithm/value_policies/subtract.hpp"
#include <pybind11/pybind11.h>


namespace lue::framework {
    namespace {

        template<typename Element>
        constexpr void define_subtract_overloads(pybind11::module& module)
        {
            using namespace lue::value_policies;
            Rank const rank{2};
            using Array = PartitionedArray<Element, rank>;
            using Scalar = Scalar<Element>;
            using Value = Element;

            module.def(
                "subtract",
                [](Array const& array1, Array const& array2) { return subtract(array1, array2); });
            module.def(
                "subtract", [](Array const& array, Scalar const& scalar) { return subtract(array, scalar); });
            module.def(
                "subtract", [](Array const& array, Value const value) { return subtract(array, value); });

            module.def(
                "subtract", [](Scalar const& scalar, Array const& array) { return subtract(scalar, array); });
            module.def(
                "subtract",
                [](Scalar const& scalar1, Scalar const& scalar2) { return subtract(scalar1, scalar2); });
            module.def(
                "subtract", [](Scalar const& scalar, Value const value) { return subtract(scalar, value); });

            module.def(
                "subtract", [](Value const value, Array const& array) { return subtract(value, array); });
            module.def(
                "subtract", [](Value const value, Scalar const& scalar) { return subtract(value, scalar); });

            module.def(
                "subtract", [](Value const value1, Value const value2) { return subtract(value1, value2); });
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
