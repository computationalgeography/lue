#include "lue/framework/algorithm/value_policies/multiply.hpp"
#include <pybind11/pybind11.h>


namespace lue::framework {
    namespace {

        template<typename Element>
        constexpr void define_multiply_overloads(pybind11::module& module)
        {
            using namespace lue::value_policies;
            Rank const rank{2};
            using Array = PartitionedArray<Element, rank>;
            using Scalar = Scalar<Element>;
            using Value = Element;

            module.def(
                "multiply",
                [](Array const& array1, Array const& array2) { return multiply(array1, array2); });
            module.def(
                "multiply", [](Array const& array, Scalar const& scalar) { return multiply(array, scalar); });
            module.def(
                "multiply", [](Array const& array, Value const value) { return multiply(array, value); });

            module.def(
                "multiply", [](Scalar const& scalar, Array const& array) { return multiply(scalar, array); });
            module.def(
                "multiply",
                [](Scalar const& scalar1, Scalar const& scalar2) { return multiply(scalar1, scalar2); });
            module.def(
                "multiply", [](Scalar const& scalar, Value const value) { return multiply(scalar, value); });

            module.def(
                "multiply", [](Value const value, Array const& array) { return multiply(value, array); });
            module.def(
                "multiply", [](Value const value, Scalar const& scalar) { return multiply(value, scalar); });

            module.def(
                "multiply", [](Value const value1, Value const value2) { return multiply(value1, value2); });
        }

    }  // Anonymous namespace


    void bind_multiply(pybind11::module& module)
    {
        define_multiply_overloads<float>(module);
        define_multiply_overloads<double>(module);
    }

}  // namespace lue::framework
