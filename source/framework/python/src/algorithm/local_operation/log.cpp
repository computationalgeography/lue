#include "lue/framework/algorithm/value_policies/log.hpp"
#include <pybind11/pybind11.h>


namespace lue::framework {
    namespace {

        template<typename Element>
        constexpr void define_log_overloads(pybind11::module& module)
        {
            using namespace lue::value_policies;
            Rank const rank{2};
            using Array = PartitionedArray<Element, rank>;
            using Scalar = Scalar<Element>;
            using Value = Element;

            module.def("log", [](Array const& array) { return log(array); });
            module.def("log", [](Scalar const& scalar) { return log(scalar); });
            module.def("log", [](Value const value) { return log(value); });
        }

    }  // Anonymous namespace


    void bind_log(pybind11::module& module)
    {
        define_log_overloads<float>(module);
        define_log_overloads<double>(module);
    }

}  // namespace lue::framework
