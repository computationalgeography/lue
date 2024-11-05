#include "lue/framework/algorithm/value_policies/comparison.hpp"
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
                    using namespace value_policies;
                    Rank const rank{2};
                    using Array = PartitionedArray<Element, rank>;
                    using Scalar = Scalar<Element>;
                    using Value = Element;
                    using BooleanElement = std::uint8_t;

                    // equal_to
                    module.def(
                        "equal_to",
                        [](Array const& array1, Array const& array2)
                        { return equal_to<BooleanElement>(array1, array2); });
                    module.def(
                        "equal_to",
                        [](Array const& array, Scalar const& scalar)
                        { return equal_to<BooleanElement>(array, scalar); });
                    module.def(
                        "equal_to",
                        [](Array const& array, Value const value)
                        { return equal_to<BooleanElement>(array, value); });

                    module.def(
                        "equal_to",
                        [](Scalar const& scalar, Array const& array)
                        { return equal_to<BooleanElement>(scalar, array); });
                    module.def(
                        "equal_to",
                        [](Scalar const& scalar1, Scalar const& scalar2)
                        { return equal_to<BooleanElement>(scalar1, scalar2); });
                    module.def(
                        "equal_to",
                        [](Scalar const& scalar, Value const value)
                        { return equal_to<BooleanElement>(scalar, value); });

                    module.def(
                        "equal_to",
                        [](Value const value, Array const& array)
                        { return equal_to<BooleanElement>(value, array); });
                    module.def(
                        "equal_to",
                        [](Value const value, Scalar const& scalar)
                        { return equal_to<BooleanElement>(value, scalar); });

                    module.def(
                        "equal_to",
                        [](Value const value1, Value const value2)
                        { return equal_to<BooleanElement>(value1, value2); });

                    // greater_than
                    module.def(
                        "greater_than",
                        [](Array const& array1, Array const& array2)
                        { return greater_than<BooleanElement>(array1, array2); });
                    module.def(
                        "greater_than",
                        [](Array const& array, Scalar const& scalar)
                        { return greater_than<BooleanElement>(array, scalar); });
                    module.def(
                        "greater_than",
                        [](Array const& array, Value const value)
                        { return greater_than<BooleanElement>(array, value); });

                    module.def(
                        "greater_than",
                        [](Scalar const& scalar, Array const& array)
                        { return greater_than<BooleanElement>(scalar, array); });
                    module.def(
                        "greater_than",
                        [](Scalar const& scalar1, Scalar const& scalar2)
                        { return greater_than<BooleanElement>(scalar1, scalar2); });
                    module.def(
                        "greater_than",
                        [](Scalar const& scalar, Value const value)
                        { return greater_than<BooleanElement>(scalar, value); });

                    module.def(
                        "greater_than",
                        [](Value const value, Array const& array)
                        { return greater_than<BooleanElement>(value, array); });
                    module.def(
                        "greater_than",
                        [](Value const value, Scalar const& scalar)
                        { return greater_than<BooleanElement>(value, scalar); });

                    module.def(
                        "greater_than",
                        [](Value const value1, Value const value2)
                        { return greater_than<BooleanElement>(value1, value2); });

                    // greater_than_equal_to
                    module.def(
                        "greater_than_equal_to",
                        [](Array const& array1, Array const& array2)
                        { return greater_than_equal_to<BooleanElement>(array1, array2); });
                    module.def(
                        "greater_than_equal_to",
                        [](Array const& array, Scalar const& scalar)
                        { return greater_than_equal_to<BooleanElement>(array, scalar); });
                    module.def(
                        "greater_than_equal_to",
                        [](Array const& array, Value const value)
                        { return greater_than_equal_to<BooleanElement>(array, value); });

                    module.def(
                        "greater_than_equal_to",
                        [](Scalar const& scalar, Array const& array)
                        { return greater_than_equal_to<BooleanElement>(scalar, array); });
                    module.def(
                        "greater_than_equal_to",
                        [](Scalar const& scalar1, Scalar const& scalar2)
                        { return greater_than_equal_to<BooleanElement>(scalar1, scalar2); });
                    module.def(
                        "greater_than_equal_to",
                        [](Scalar const& scalar, Value const value)
                        { return greater_than_equal_to<BooleanElement>(scalar, value); });

                    module.def(
                        "greater_than_equal_to",
                        [](Value const value, Array const& array)
                        { return greater_than_equal_to<BooleanElement>(value, array); });
                    module.def(
                        "greater_than_equal_to",
                        [](Value const value, Scalar const& scalar)
                        { return greater_than_equal_to<BooleanElement>(value, scalar); });

                    module.def(
                        "greater_than_equal_to",
                        [](Value const value1, Value const value2)
                        { return greater_than_equal_to<BooleanElement>(value1, value2); });

                    // less_than
                    module.def(
                        "less_than",
                        [](Array const& array1, Array const& array2)
                        { return less_than<BooleanElement>(array1, array2); });
                    module.def(
                        "less_than",
                        [](Array const& array, Scalar const& scalar)
                        { return less_than<BooleanElement>(array, scalar); });
                    module.def(
                        "less_than",
                        [](Array const& array, Value const value)
                        { return less_than<BooleanElement>(array, value); });

                    module.def(
                        "less_than",
                        [](Scalar const& scalar, Array const& array)
                        { return less_than<BooleanElement>(scalar, array); });
                    module.def(
                        "less_than",
                        [](Scalar const& scalar1, Scalar const& scalar2)
                        { return less_than<BooleanElement>(scalar1, scalar2); });
                    module.def(
                        "less_than",
                        [](Scalar const& scalar, Value const value)
                        { return less_than<BooleanElement>(scalar, value); });

                    module.def(
                        "less_than",
                        [](Value const value, Array const& array)
                        { return less_than<BooleanElement>(value, array); });
                    module.def(
                        "less_than",
                        [](Value const value, Scalar const& scalar)
                        { return less_than<BooleanElement>(value, scalar); });

                    module.def(
                        "less_than",
                        [](Value const value1, Value const value2)
                        { return less_than<BooleanElement>(value1, value2); });

                    // less_than_equal_to
                    module.def(
                        "less_than_equal_to",
                        [](Array const& array1, Array const& array2)
                        { return less_than_equal_to<BooleanElement>(array1, array2); });
                    module.def(
                        "less_than_equal_to",
                        [](Array const& array, Scalar const& scalar)
                        { return less_than_equal_to<BooleanElement>(array, scalar); });
                    module.def(
                        "less_than_equal_to",
                        [](Array const& array, Value const value)
                        { return less_than_equal_to<BooleanElement>(array, value); });

                    module.def(
                        "less_than_equal_to",
                        [](Scalar const& scalar, Array const& array)
                        { return less_than_equal_to<BooleanElement>(scalar, array); });
                    module.def(
                        "less_than_equal_to",
                        [](Scalar const& scalar1, Scalar const& scalar2)
                        { return less_than_equal_to<BooleanElement>(scalar1, scalar2); });
                    module.def(
                        "less_than_equal_to",
                        [](Scalar const& scalar, Value const value)
                        { return less_than_equal_to<BooleanElement>(scalar, value); });

                    module.def(
                        "less_than_equal_to",
                        [](Value const value, Array const& array)
                        { return less_than_equal_to<BooleanElement>(value, array); });
                    module.def(
                        "less_than_equal_to",
                        [](Value const value, Scalar const& scalar)
                        { return less_than_equal_to<BooleanElement>(value, scalar); });

                    module.def(
                        "less_than_equal_to",
                        [](Value const value1, Value const value2)
                        { return less_than_equal_to<BooleanElement>(value1, value2); });

                    // not_equal_to
                    module.def(
                        "not_equal_to",
                        [](Array const& array1, Array const& array2)
                        { return not_equal_to<BooleanElement>(array1, array2); });
                    module.def(
                        "not_equal_to",
                        [](Array const& array, Scalar const& scalar)
                        { return not_equal_to<BooleanElement>(array, scalar); });
                    module.def(
                        "not_equal_to",
                        [](Array const& array, Value const value)
                        { return not_equal_to<BooleanElement>(array, value); });

                    module.def(
                        "not_equal_to",
                        [](Scalar const& scalar, Array const& array)
                        { return not_equal_to<BooleanElement>(scalar, array); });
                    module.def(
                        "not_equal_to",
                        [](Scalar const& scalar1, Scalar const& scalar2)
                        { return not_equal_to<BooleanElement>(scalar1, scalar2); });
                    module.def(
                        "not_equal_to",
                        [](Scalar const& scalar, Value const value)
                        { return not_equal_to<BooleanElement>(scalar, value); });

                    module.def(
                        "not_equal_to",
                        [](Value const value, Array const& array)
                        { return not_equal_to<BooleanElement>(value, array); });
                    module.def(
                        "not_equal_to",
                        [](Value const value, Scalar const& scalar)
                        { return not_equal_to<BooleanElement>(value, scalar); });

                    module.def(
                        "not_equal_to",
                        [](Value const value1, Value const value2)
                        { return not_equal_to<BooleanElement>(value1, value2); });
                }
        };

    }  // Anonymous namespace


    void bind_comparison(pybind11::module& module)
    {
        bind<Binder, ArithmeticElements>(module);
    }

}  // namespace lue::framework
