#include "lue/framework/algorithm/value_policies/pow.hpp"
#include <pybind11/pybind11.h>


namespace lue::framework {
    namespace {

        template<typename Element>
        constexpr void define_pow_overloads(pybind11::module& module)
        {
            using namespace lue::value_policies;
            Rank const rank{2};
            using Array = PartitionedArray<Element, rank>;
            using Scalar = Scalar<Element>;
            using Value = Element;

            module.def("pow", [](Array const& array1, Array const& array2) { return pow(array1, array2); });
            module.def("pow", [](Array const& array, Scalar const& scalar) { return pow(array, scalar); });
            module.def("pow", [](Array const& array, Value const value) { return pow(array, value); });

            module.def("pow", [](Scalar const& scalar, Array const& array) { return pow(scalar, array); });
            module.def(
                "pow", [](Scalar const& scalar1, Scalar const& scalar2) { return pow(scalar1, scalar2); });
            module.def("pow", [](Scalar const& scalar, Value const value) { return pow(scalar, value); });

            module.def("pow", [](Value const value, Array const& array) { return pow(value, array); });
            module.def("pow", [](Value const value, Scalar const& scalar) { return pow(value, scalar); });

            module.def("pow", [](Value const value1, Value const value2) { return pow(value1, value2); });
        }

    }  // Anonymous namespace


    void bind_pow(pybind11::module& module)
    {
        define_pow_overloads<float>(module);
        define_pow_overloads<double>(module);
    }

}  // namespace lue::framework
