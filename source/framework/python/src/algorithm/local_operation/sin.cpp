#include "lue/framework/algorithm/value_policies/sin.hpp"
#include <pybind11/pybind11.h>


namespace lue::framework {
    namespace {

        template<typename Element>
        constexpr void define_sin_overloads(pybind11::module& module)
        {
            using namespace lue::value_policies;
            Rank const rank{2};
            using Array = PartitionedArray<Element, rank>;
            using Scalar = Scalar<Element>;
            using Value = Element;

            module.def("sin", [](Array const& array) { return sin(array); });
            module.def("sin", [](Scalar const& scalar) { return sin(scalar); });
            module.def("sin", [](Value const value) { return sin(value); });
        }

    }  // Anonymous namespace


    void bind_sin(pybind11::module& module)
    {
        define_sin_overloads<float>(module);
        define_sin_overloads<double>(module);
    }

}  // namespace lue::framework
