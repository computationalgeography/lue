#include "lue/framework/algorithm/value_policies/floor.hpp"
#include <pybind11/pybind11.h>


namespace lue::framework {
    namespace {

        template<typename Element>
        constexpr void define_floor_overloads(pybind11::module& module)
        {
            using namespace lue::value_policies;
            Rank const rank{2};
            using Array = PartitionedArray<Element, rank>;
            using Scalar = Scalar<Element>;
            using Value = Element;

            module.def("floor", [](Array const& array) { return floor(array); });
            module.def("floor", [](Scalar const& scalar) { return floor(scalar); });
            module.def("floor", [](Value const value) { return floor(value); });
        }

    }  // Anonymous namespace


    void bind_floor(pybind11::module& module)
    {
        define_floor_overloads<float>(module);
        define_floor_overloads<double>(module);
    }

}  // namespace lue::framework
