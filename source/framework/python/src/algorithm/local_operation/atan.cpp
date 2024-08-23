#include "lue/framework/algorithm/value_policies/atan.hpp"
#include <pybind11/pybind11.h>


namespace lue::framework {
    namespace {

        template<typename Element>
        constexpr void define_atan_overloads(pybind11::module& module)
        {
            using namespace lue::value_policies;
            Rank const rank{2};
            using Array = PartitionedArray<Element, rank>;
            using Scalar = Scalar<Element>;
            using Value = Element;

            module.def("atan", [](Array const& array) { return atan(array); });
            module.def("atan", [](Scalar const& scalar) { return atan(scalar); });
            module.def("atan", [](Value const value) { return atan(value); });
        }

    }  // Anonymous namespace


    void bind_atan(pybind11::module& module)
    {
        define_atan_overloads<float>(module);
        define_atan_overloads<double>(module);
    }

}  // namespace lue::framework
