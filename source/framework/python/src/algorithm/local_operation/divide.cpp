#include "lue/framework/algorithm/value_policies/divide.hpp"
#include <pybind11/pybind11.h>


namespace lue::framework {
    namespace {

        template<typename Element>
        constexpr void define_divide_overloads(pybind11::module& module)
        {
            using namespace lue::value_policies;
            Rank const rank{2};
            using Array = PartitionedArray<Element, rank>;
            using Scalar = Scalar<Element>;
            using Value = Element;

            module.def(
                "divide", [](Array const& array1, Array const& array2) { return divide(array1, array2); });
            module.def(
                "divide", [](Array const& array, Scalar const& scalar) { return divide(array, scalar); });
            module.def("divide", [](Array const& array, Value const value) { return divide(array, value); });

            module.def(
                "divide", [](Scalar const& scalar, Array const& array) { return divide(scalar, array); });
            module.def(
                "divide",
                [](Scalar const& scalar1, Scalar const& scalar2) { return divide(scalar1, scalar2); });
            module.def(
                "divide", [](Scalar const& scalar, Value const value) { return divide(scalar, value); });

            module.def("divide", [](Value const value, Array const& array) { return divide(value, array); });
            module.def(
                "divide", [](Value const value, Scalar const& scalar) { return divide(value, scalar); });

            module.def(
                "divide", [](Value const value1, Value const value2) { return divide(value1, value2); });
        }

    }  // Anonymous namespace


    void bind_divide(pybind11::module& module)
    {
        define_divide_overloads<float>(module);
        define_divide_overloads<double>(module);
    }

}  // namespace lue::framework
