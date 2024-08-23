#include "lue/framework/algorithm/value_policies/sqrt.hpp"
#include <pybind11/pybind11.h>


namespace lue::framework {
    namespace {

        template<typename Element>
        constexpr void define_sqrt_overloads(pybind11::module& module)
        {
            using namespace lue::value_policies;
            Rank const rank{2};
            using Array = PartitionedArray<Element, rank>;
            using Scalar = Scalar<Element>;
            using Value = Element;

            module.def("sqrt", [](Array const& array) { return sqrt(array); });
            module.def("sqrt", [](Scalar const& scalar) { return sqrt(scalar); });
            module.def("sqrt", [](Value const value) { return sqrt(value); });
        }

    }  // Anonymous namespace


    void bind_sqrt(pybind11::module& module)
    {
        define_sqrt_overloads<float>(module);
        define_sqrt_overloads<double>(module);
    }

}  // namespace lue::framework
