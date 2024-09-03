#include "lue/framework/algorithm/value_policies/cos.hpp"
#include <pybind11/pybind11.h>


namespace lue::framework {
    namespace {

        template<typename Element>
        constexpr void define_cos_overloads(pybind11::module& module)
        {
            using namespace lue::value_policies;
            Rank const rank{2};
            using Array = PartitionedArray<Element, rank>;
            using Scalar = Scalar<Element>;
            using Value = Element;

            module.def("cos", [](Array const& array) { return cos(array); });
            module.def("cos", [](Scalar const& scalar) { return cos(scalar); });
            module.def("cos", [](Value const value) { return cos(value); });
        }

    }  // Anonymous namespace


    void bind_cos(pybind11::module& module)
    {
        define_cos_overloads<float>(module);
        define_cos_overloads<double>(module);
    }

}  // namespace lue::framework
