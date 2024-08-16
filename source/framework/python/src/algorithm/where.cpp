#include "lue/framework/algorithm/value_policies/where.hpp"
#include <pybind11/pybind11.h>


namespace lue::framework {
    namespace {

        template<typename Element>
        constexpr void define_where_overloads(pybind11::module& module)
        {
            using namespace lue::value_policies;

            Rank const rank{2};

            using ConditionElement = std::uint8_t;
            using ConditionArray = PartitionedArray<ConditionElement, rank>;
            using Array = PartitionedArray<Element, rank>;
            using Scalar = Scalar<Element>;
            using Value = Element;

            module.def(
                "where",
                [](ConditionArray const& condition_array, Array const& true_array)
                { return where(condition_array, true_array); });
            module.def(
                "where",
                [](ConditionArray const& condition_array, Scalar const& true_scalar)
                { return where(condition_array, true_scalar); });
            module.def(
                "where",
                [](ConditionArray const& condition_array, Value const true_value)
                { return where(condition_array, true_value); });

            module.def(
                "where",
                [](ConditionArray const& condition_array, Array const& true_array, Array const& false_array)
                { return where(condition_array, true_array, false_array); });
            module.def(
                "where",
                [](ConditionArray const& condition_array, Scalar const& true_scalar, Array const& false_array)
                { return where(condition_array, true_scalar, false_array); });
            module.def(
                "where",
                [](ConditionArray const& condition_array, Value const true_value, Array const& false_array)
                { return where(condition_array, true_value, false_array); });

            module.def(
                "where",
                [](ConditionArray const& condition_array, Array const& true_array, Array const& false_array)
                { return where(condition_array, true_array, false_array); });
            module.def(
                "where",
                [](ConditionArray const& condition_array, Array const& true_array, Scalar const& false_scalar)
                { return where(condition_array, true_array, false_scalar); });
            module.def(
                "where",
                [](ConditionArray const& condition_array, Array const& true_array, Value const false_value)
                { return where(condition_array, true_array, false_value); });

            module.def(
                "where",
                [](ConditionArray const& condition_array,
                   Scalar const& true_scalar,
                   Scalar const& false_scalar) { return where(condition_array, true_scalar, false_scalar); });
            module.def(
                "where",
                [](ConditionArray const& condition_array, Value const true_value, Value const false_value)
                { return where(condition_array, true_value, false_value); });
        }

    }  // Anonymous namespace


    void bind_where(pybind11::module& module)
    {
        define_where_overloads<std::uint8_t>(module);
        define_where_overloads<std::uint32_t>(module);
        define_where_overloads<std::uint64_t>(module);
        define_where_overloads<std::int32_t>(module);
        define_where_overloads<std::int64_t>(module);
        define_where_overloads<float>(module);
        define_where_overloads<double>(module);
    }

}  // namespace lue::framework
