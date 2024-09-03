#include "lue/framework/algorithm/value_policies/asin.hpp"
#include <pybind11/pybind11.h>


namespace lue::framework {
    namespace {

        template<typename Element>
        constexpr void define_asin_overloads(pybind11::module& module)
        {
            using namespace lue::value_policies;
            Rank const rank{2};
            using Array = PartitionedArray<Element, rank>;
            using Scalar = Scalar<Element>;
            using Value = Element;

            module.def("asin", [](Array const& array) { return asin(array); });
            module.def("asin", [](Scalar const& scalar) { return asin(scalar); });
            module.def("asin", [](Value const value) { return asin(value); });
        }

    }  // Anonymous namespace


    void bind_asin(pybind11::module& module)
    {
        define_asin_overloads<float>(module);
        define_asin_overloads<double>(module);
    }

}  // namespace lue::framework
