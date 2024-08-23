#include "lue/framework/algorithm/value_policies/acos.hpp"
#include <pybind11/pybind11.h>


namespace lue::framework {
    namespace {

        template<typename Element>
        constexpr void define_acos_overloads(pybind11::module& module)
        {
            using namespace lue::value_policies;
            Rank const rank{2};
            using Array = PartitionedArray<Element, rank>;
            using Scalar = Scalar<Element>;
            using Value = Element;

            module.def("acos", [](Array const& array) { return acos(array); });
            module.def("acos", [](Scalar const& scalar) { return acos(scalar); });
            module.def("acos", [](Value const value) { return acos(value); });
        }

    }  // Anonymous namespace


    void bind_acos(pybind11::module& module)
    {
        define_acos_overloads<float>(module);
        define_acos_overloads<double>(module);
    }

}  // namespace lue::framework
