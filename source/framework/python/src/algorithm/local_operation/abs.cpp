#include "lue/framework/algorithm/value_policies/abs.hpp"
#include <pybind11/pybind11.h>


namespace lue::framework {
    namespace {

        template<typename Element>
        constexpr void define_abs_overloads(pybind11::module& module)
        {
            using namespace lue::value_policies;
            Rank const rank{2};
            using Array = PartitionedArray<Element, rank>;
            using Scalar = Scalar<Element>;
            using Value = Element;

            module.def("abs", [](Array const& array) { return abs(array); });
            module.def("abs", [](Scalar const& scalar) { return abs(scalar); });
            module.def("abs", [](Value const value) { return abs(value); });
        }

    }  // Anonymous namespace

    void bind_abs(pybind11::module& module)
    {
        define_abs_overloads<std::int32_t>(module);
        define_abs_overloads<std::int64_t>(module);
        define_abs_overloads<float>(module);
        define_abs_overloads<double>(module);
    }

}  // namespace lue::framework
