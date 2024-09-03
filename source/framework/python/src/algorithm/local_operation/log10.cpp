#include "lue/framework/algorithm/value_policies/log10.hpp"
#include <pybind11/pybind11.h>


namespace lue::framework {
    namespace {

        template<typename Element>
        constexpr void define_log10_overloads(pybind11::module& module)
        {
            using namespace lue::value_policies;
            Rank const rank{2};
            using Array = PartitionedArray<Element, rank>;
            using Scalar = Scalar<Element>;
            using Value = Element;

            module.def("log10", [](Array const& array) { return log10(array); });
            module.def("log10", [](Scalar const& scalar) { return log10(scalar); });
            module.def("log10", [](Value const value) { return log10(value); });
        }

    }  // Anonymous namespace


    void bind_log10(pybind11::module& module)
    {
        define_log10_overloads<float>(module);
        define_log10_overloads<double>(module);
    }

}  // namespace lue::framework
