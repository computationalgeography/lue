#include "lue/framework/algorithm/value_policies/modulus.hpp"
#include <pybind11/pybind11.h>


namespace lue::framework {
    namespace {

        template<typename Element>
        constexpr void define_modulus_overloads(pybind11::module& module)
        {
            using namespace lue::value_policies;
            Rank const rank{2};
            using Array = PartitionedArray<Element, rank>;
            using Scalar = Scalar<Element>;
            using Value = Element;

            module.def(
                "modulus",
                [](Array const& array1, Array const& array2) { return modulus(array1, array2); });
            module.def(
                "modulus", [](Array const& array, Scalar const& scalar) { return modulus(array, scalar); });
            module.def(
                "modulus", [](Array const& array, Value const value) { return modulus(array, value); });

            module.def(
                "modulus", [](Scalar const& scalar, Array const& array) { return modulus(scalar, array); });
            module.def(
                "modulus",
                [](Scalar const& scalar1, Scalar const& scalar2) { return modulus(scalar1, scalar2); });
            module.def(
                "modulus", [](Scalar const& scalar, Value const value) { return modulus(scalar, value); });

            module.def(
                "modulus", [](Value const value, Array const& array) { return modulus(value, array); });
            module.def(
                "modulus", [](Value const value, Scalar const& scalar) { return modulus(value, scalar); });

            module.def(
                "modulus", [](Value const value1, Value const value2) { return modulus(value1, value2); });
        }

    }  // Anonymous namespace


    void bind_modulus(pybind11::module& module)
    {
        define_modulus_overloads<std::int32_t>(module);
        define_modulus_overloads<std::int64_t>(module);
    }

}  // namespace lue::framework
