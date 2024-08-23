#include "lue/framework/algorithm/value_policies/round.hpp"
#include <pybind11/pybind11.h>


namespace lue::framework {
    namespace {

        template<typename Element>
        constexpr void define_round_overloads(pybind11::module& module)
        {
            using namespace lue::value_policies;
            Rank const rank{2};
            using Array = PartitionedArray<Element, rank>;
            using Scalar = Scalar<Element>;
            using Value = Element;

            module.def("round", [](Array const& array) { return round(array); });
            module.def("round", [](Scalar const& scalar) { return round(scalar); });
            module.def("round", [](Value const value) { return round(value); });
        }

    }  // Anonymous namespace


    void bind_round(pybind11::module& module)
    {
        define_round_overloads<float>(module);
        define_round_overloads<double>(module);
    }

}  // namespace lue::framework
