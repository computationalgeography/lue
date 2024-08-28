#include "lue/framework/algorithm/value_policies/all.hpp"
#include <pybind11/pybind11.h>


namespace lue::framework {
    namespace {

        template<typename Element>
        constexpr void define_all_overloads(pybind11::module& module)
        {
            using namespace lue::value_policies;
            Rank const rank{2};
            using Array = PartitionedArray<Element, rank>;

            module.def("all", [](Array const& array) { return all(array); });
        }

    }  // Anonymous namespace


    void bind_all(pybind11::module& module)
    {
        define_all_overloads<std::uint8_t>(module);
        define_all_overloads<std::int32_t>(module);
        define_all_overloads<std::uint32_t>(module);
        define_all_overloads<std::int64_t>(module);
        define_all_overloads<std::uint64_t>(module);
        define_all_overloads<float>(module);
        define_all_overloads<double>(module);
    }

}  // namespace lue::framework
