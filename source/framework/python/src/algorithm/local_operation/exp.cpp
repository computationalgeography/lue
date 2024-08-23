#include "lue/framework/algorithm/value_policies/exp.hpp"
#include <pybind11/pybind11.h>


namespace lue::framework {
    namespace {

        template<typename Element>
        constexpr void define_exp_overloads(pybind11::module& module)
        {
            using namespace lue::value_policies;
            Rank const rank{2};
            using Array = PartitionedArray<Element, rank>;
            using Scalar = Scalar<Element>;
            using Value = Element;

            module.def("exp", [](Array const& array) { return exp(array); });
            module.def("exp", [](Scalar const& scalar) { return exp(scalar); });
            module.def("exp", [](Value const value) { return exp(value); });
        }

    }  // Anonymous namespace


    void bind_exp(pybind11::module& module)
    {
        define_exp_overloads<float>(module);
        define_exp_overloads<double>(module);
    }

}  // namespace lue::framework
