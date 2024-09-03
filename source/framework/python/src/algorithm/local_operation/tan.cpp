#include "lue/framework/algorithm/value_policies/tan.hpp"
#include <pybind11/pybind11.h>


namespace lue::framework {
    namespace {

        template<typename Element>
        constexpr void define_tan_overloads(pybind11::module& module)
        {
            using namespace lue::value_policies;
            Rank const rank{2};
            using Array = PartitionedArray<Element, rank>;
            using Scalar = Scalar<Element>;
            using Value = Element;

            module.def("tan", [](Array const& array) { return tan(array); });
            module.def("tan", [](Scalar const& scalar) { return tan(scalar); });
            module.def("tan", [](Value const value) { return tan(value); });
        }

    }  // Anonymous namespace


    void bind_tan(pybind11::module& module)
    {
        define_tan_overloads<float>(module);
        define_tan_overloads<double>(module);
    }

}  // namespace lue::framework
