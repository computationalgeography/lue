#include "lue/framework/algorithm/value_policies/negate.hpp"
#include <pybind11/pybind11.h>


namespace lue::framework {
    namespace {

        template<typename Element>
        constexpr void define_negate_overloads(pybind11::module& module)
        {
            using namespace lue::value_policies;
            Rank const rank{2};
            using Array = PartitionedArray<Element, rank>;
            using Scalar = Scalar<Element>;
            using Value = Element;

            module.def("negate", [](Array const& array) { return negate(array); });
            module.def("negate", [](Scalar const& scalar) { return negate(scalar); });
            module.def("negate", [](Value const value) { return negate(value); });
        }

    }  // Anonymous namespace


    void bind_negate(pybind11::module& module)
    {
        define_negate_overloads<std::int32_t>(module);
        define_negate_overloads<std::int64_t>(module);
        define_negate_overloads<float>(module);
        define_negate_overloads<double>(module);
    }

}  // namespace lue::framework
