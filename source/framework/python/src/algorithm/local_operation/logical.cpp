#include "lue/framework/algorithm/value_policies/logical.hpp"
#include "bind.hpp"
#include "lue/framework.hpp"


namespace lue::framework {
    namespace {

        class Binder
        {

            public:

                template<typename Element>
                static void bind(pybind11::module& module)
                {
                    using namespace value_policies;
                    Rank const rank{2};
                    using Array = PartitionedArray<Element, rank>;
                    using Scalar = Scalar<Element>;
                    using Value = Element;

                    // logical_and
                    module.def(
                        "logical_and",
                        [](Array const& array1, Array const& array2)
                        { return logical_and<BooleanElement>(array1, array2); });
                    module.def(
                        "logical_and",
                        [](Array const& array, Scalar const& scalar)
                        { return logical_and<BooleanElement>(array, scalar); });
                    module.def(
                        "logical_and",
                        [](Array const& array, Value const value)
                        { return logical_and<BooleanElement>(array, value); });

                    module.def(
                        "logical_and",
                        [](Scalar const& scalar, Array const& array)
                        { return logical_and<BooleanElement>(scalar, array); });
                    module.def(
                        "logical_and",
                        [](Scalar const& scalar1, Scalar const& scalar2)
                        { return logical_and<BooleanElement>(scalar1, scalar2); });
                    module.def(
                        "logical_and",
                        [](Scalar const& scalar, Value const value)
                        { return logical_and<BooleanElement>(scalar, value); });

                    module.def(
                        "logical_and",
                        [](Value const value, Array const& array)
                        { return logical_and<BooleanElement>(value, array); });
                    module.def(
                        "logical_and",
                        [](Value const value, Scalar const& scalar)
                        { return logical_and<BooleanElement>(value, scalar); });

                    module.def(
                        "logical_and",
                        [](Value const value1, Value const value2)
                        { return logical_and<BooleanElement>(value1, value2); });

                    // logical_exclusive_or
                    module.def(
                        "logical_exclusive_or",
                        [](Array const& array1, Array const& array2)
                        { return logical_exclusive_or<BooleanElement>(array1, array2); });
                    module.def(
                        "logical_exclusive_or",
                        [](Array const& array, Scalar const& scalar)
                        { return logical_exclusive_or<BooleanElement>(array, scalar); });
                    module.def(
                        "logical_exclusive_or",
                        [](Array const& array, Value const value)
                        { return logical_exclusive_or<BooleanElement>(array, value); });

                    module.def(
                        "logical_exclusive_or",
                        [](Scalar const& scalar, Array const& array)
                        { return logical_exclusive_or<BooleanElement>(scalar, array); });
                    module.def(
                        "logical_exclusive_or",
                        [](Scalar const& scalar1, Scalar const& scalar2)
                        { return logical_exclusive_or<BooleanElement>(scalar1, scalar2); });
                    module.def(
                        "logical_exclusive_or",
                        [](Scalar const& scalar, Value const value)
                        { return logical_exclusive_or<BooleanElement>(scalar, value); });

                    module.def(
                        "logical_exclusive_or",
                        [](Value const value, Array const& array)
                        { return logical_exclusive_or<BooleanElement>(value, array); });
                    module.def(
                        "logical_exclusive_or",
                        [](Value const value, Scalar const& scalar)
                        { return logical_exclusive_or<BooleanElement>(value, scalar); });

                    module.def(
                        "logical_exclusive_or",
                        [](Value const value1, Value const value2)
                        { return logical_exclusive_or<BooleanElement>(value1, value2); });

                    // inclusive_or
                    module.def(
                        "logical_inclusive_or",
                        [](Array const& array1, Array const& array2)
                        { return logical_inclusive_or<BooleanElement>(array1, array2); });
                    module.def(
                        "logical_inclusive_or",
                        [](Array const& array, Scalar const& scalar)
                        { return logical_inclusive_or<BooleanElement>(array, scalar); });
                    module.def(
                        "logical_inclusive_or",
                        [](Array const& array, Value const value)
                        { return logical_inclusive_or<BooleanElement>(array, value); });

                    module.def(
                        "logical_inclusive_or",
                        [](Scalar const& scalar, Array const& array)
                        { return logical_inclusive_or<BooleanElement>(scalar, array); });
                    module.def(
                        "logical_inclusive_or",
                        [](Scalar const& scalar1, Scalar const& scalar2)
                        { return logical_inclusive_or<BooleanElement>(scalar1, scalar2); });
                    module.def(
                        "logical_inclusive_or",
                        [](Scalar const& scalar, Value const value)
                        { return logical_inclusive_or<BooleanElement>(scalar, value); });

                    module.def(
                        "logical_inclusive_or",
                        [](Value const value, Array const& array)
                        { return logical_inclusive_or<BooleanElement>(value, array); });
                    module.def(
                        "logical_inclusive_or",
                        [](Value const value, Scalar const& scalar)
                        { return logical_inclusive_or<BooleanElement>(value, scalar); });

                    module.def(
                        "logical_inclusive_or",
                        [](Value const value1, Value const value2)
                        { return logical_inclusive_or<BooleanElement>(value1, value2); });

                    // logical_not
                    module.def(
                        "logical_not", [](Array const& array) { return logical_not<BooleanElement>(array); });
                    module.def(
                        "logical_not",
                        [](Scalar const& scalar) { return logical_not<BooleanElement>(scalar); });
                    module.def(
                        "logical_not", [](Value const value) { return logical_not<BooleanElement>(value); });
                }
        };

    }  // Anonymous namespace


    void bind_logical(pybind11::module& module)
    {
        bind<Binder, IntegralElements>(module);
    }

}  // namespace lue::framework
