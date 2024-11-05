#include "lue/framework/algorithm/value_policies/where.hpp"
#include "bind.hpp"
#include "lue/framework.hpp"


namespace lue::framework {
    namespace {

        class Binder
        {

            public:

                template<Arithmetic Element>
                static void bind(pybind11::module& module)
                {
                    using namespace lue::value_policies;

                    Rank const rank{2};

                    using ConditionArray = PartitionedArray<BooleanElement, rank>;
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
                        [](ConditionArray const& condition_array,
                           Array const& true_array,
                           Array const& false_array)
                        { return where(condition_array, true_array, false_array); });
                    module.def(
                        "where",
                        [](ConditionArray const& condition_array,
                           Scalar const& true_scalar,
                           Array const& false_array)
                        { return where(condition_array, true_scalar, false_array); });
                    module.def(
                        "where",
                        [](ConditionArray const& condition_array,
                           Value const true_value,
                           Array const& false_array)
                        { return where(condition_array, true_value, false_array); });

                    module.def(
                        "where",
                        [](ConditionArray const& condition_array,
                           Array const& true_array,
                           Array const& false_array)
                        { return where(condition_array, true_array, false_array); });
                    module.def(
                        "where",
                        [](ConditionArray const& condition_array,
                           Array const& true_array,
                           Scalar const& false_scalar)
                        { return where(condition_array, true_array, false_scalar); });
                    module.def(
                        "where",
                        [](ConditionArray const& condition_array,
                           Array const& true_array,
                           Value const false_value)
                        { return where(condition_array, true_array, false_value); });

                    module.def(
                        "where",
                        [](ConditionArray const& condition_array,
                           Scalar const& true_scalar,
                           Scalar const& false_scalar)
                        { return where(condition_array, true_scalar, false_scalar); });
                    module.def(
                        "where",
                        [](ConditionArray const& condition_array,
                           Value const true_value,
                           Value const false_value)
                        { return where(condition_array, true_value, false_value); });
                }
        };

    }  // Anonymous namespace


    void bind_where(pybind11::module& module)
    {
        bind<Binder, ArithmeticElements>(module);
    }

}  // namespace lue::framework
