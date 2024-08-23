#include "lue/framework/algorithm/value_policies/valid.hpp"
#include <pybind11/pybind11.h>


namespace lue::framework {
    namespace {

        template<typename Element>
        constexpr void define_valid_overloads(pybind11::module& module)
        {
            using namespace lue::value_policies;
            Rank const rank{2};
            using Array = PartitionedArray<Element, rank>;
            using Scalar = Scalar<Element>;
            using Value = Element;
            using BooleanElement = std::uint8_t;

            module.def("valid", [](Array const& array) { return valid<BooleanElement>(array); });
            module.def("valid", [](Scalar const& scalar) { return valid<BooleanElement>(scalar); });
            module.def("valid", [](Value const value) { return valid<BooleanElement>(value); });
        }

    }  // Anonymous namespace


    void bind_valid(pybind11::module& module)
    {
        define_valid_overloads<std::uint8_t>(module);
        define_valid_overloads<std::int32_t>(module);
        define_valid_overloads<std::uint32_t>(module);
        define_valid_overloads<std::int64_t>(module);
        define_valid_overloads<std::uint64_t>(module);
        define_valid_overloads<float>(module);
        define_valid_overloads<double>(module);
    }

}  // namespace lue::framework
