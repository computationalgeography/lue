#include "lue/framework/algorithm/value_policies/ceil.hpp"
#include <pybind11/pybind11.h>


namespace lue::framework {
    namespace {

        template<typename Element>
        constexpr void define_ceil_overloads(pybind11::module& module)
        {
            using namespace lue::value_policies;
            Rank const rank{2};
            using Array = PartitionedArray<Element, rank>;
            using Scalar = Scalar<Element>;
            using Value = Element;

            module.def("ceil", [](Array const& array) { return ceil(array); });
            module.def("ceil", [](Scalar const& scalar) { return ceil(scalar); });
            module.def("ceil", [](Value const value) { return ceil(value); });
        }

    }  // Anonymous namespace


    void bind_ceil(pybind11::module& module)
    {
        define_ceil_overloads<float>(module);
        define_ceil_overloads<double>(module);
    }

}  // namespace lue::framework
