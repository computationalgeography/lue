#include "lue/framework/algorithm/value_policies/arithmetic.hpp"
#include "bind.hpp"
#include "lue/framework.hpp"


namespace lue::framework {
    namespace {

        class ArithmeticElementBinder
        {
            public:

                template<Arithmetic Element>
                static void bind(pybind11::module& module)
                {
                    using namespace lue::value_policies;
                    Rank const rank{2};
                    using Array = PartitionedArray<Element, rank>;
                    using Scalar = Scalar<Element>;
                    using Value = Element;

                    // add
                    module.def(
                        "add", [](Array const& array1, Array const& array2) { return add(array1, array2); });
                    module.def(
                        "add", [](Array const& array, Scalar const& scalar) { return add(array, scalar); });
                    module.def(
                        "add", [](Array const& array, Value const value) { return add(array, value); });

                    module.def(
                        "add", [](Scalar const& scalar, Array const& array) { return add(scalar, array); });
                    module.def(
                        "add",
                        [](Scalar const& scalar1, Scalar const& scalar2) { return add(scalar1, scalar2); });
                    module.def(
                        "add", [](Scalar const& scalar, Value const value) { return add(scalar, value); });

                    module.def(
                        "add", [](Value const value, Array const& array) { return add(value, array); });
                    module.def(
                        "add", [](Value const value, Scalar const& scalar) { return add(value, scalar); });

                    module.def(
                        "add", [](Value const value1, Value const value2) { return add(value1, value2); });

                    // subtract
                    module.def(
                        "subtract",
                        [](Array const& array1, Array const& array2) { return subtract(array1, array2); });
                    module.def(
                        "subtract",
                        [](Array const& array, Scalar const& scalar) { return subtract(array, scalar); });
                    module.def(
                        "subtract",
                        [](Array const& array, Value const value) { return subtract(array, value); });

                    module.def(
                        "subtract",
                        [](Scalar const& scalar, Array const& array) { return subtract(scalar, array); });
                    module.def(
                        "subtract",
                        [](Scalar const& scalar1, Scalar const& scalar2)
                        { return subtract(scalar1, scalar2); });
                    module.def(
                        "subtract",
                        [](Scalar const& scalar, Value const value) { return subtract(scalar, value); });

                    module.def(
                        "subtract",
                        [](Value const value, Array const& array) { return subtract(value, array); });
                    module.def(
                        "subtract",
                        [](Value const value, Scalar const& scalar) { return subtract(value, scalar); });

                    module.def(
                        "subtract",
                        [](Value const value1, Value const value2) { return subtract(value1, value2); });
                }
        };


        class FloatingPointElementBinder
        {
            public:

                template<std::floating_point Element>
                static void bind(pybind11::module& module)
                {
                    using namespace lue::value_policies;
                    Rank const rank{2};
                    using Array = PartitionedArray<Element, rank>;
                    using Scalar = Scalar<Element>;
                    using Value = Element;

                    // divide
                    module.def(
                        "divide",
                        [](Array const& array1, Array const& array2) { return divide(array1, array2); });
                    module.def(
                        "divide",
                        [](Array const& array, Scalar const& scalar) { return divide(array, scalar); });
                    module.def(
                        "divide", [](Array const& array, Value const value) { return divide(array, value); });

                    module.def(
                        "divide",
                        [](Scalar const& scalar, Array const& array) { return divide(scalar, array); });
                    module.def(
                        "divide",
                        [](Scalar const& scalar1, Scalar const& scalar2)
                        { return divide(scalar1, scalar2); });
                    module.def(
                        "divide",
                        [](Scalar const& scalar, Value const value) { return divide(scalar, value); });

                    module.def(
                        "divide", [](Value const value, Array const& array) { return divide(value, array); });
                    module.def(
                        "divide",
                        [](Value const value, Scalar const& scalar) { return divide(value, scalar); });

                    module.def(
                        "divide",
                        [](Value const value1, Value const value2) { return divide(value1, value2); });

                    // exp
                    module.def("exp", [](Array const& array) { return exp(array); });
                    module.def("exp", [](Scalar const& scalar) { return exp(scalar); });
                    module.def("exp", [](Value const value) { return exp(value); });

                    // log
                    module.def("log", [](Array const& array) { return log(array); });
                    module.def("log", [](Scalar const& scalar) { return log(scalar); });
                    module.def("log", [](Value const value) { return log(value); });

                    // log10
                    module.def("log10", [](Array const& array) { return log10(array); });
                    module.def("log10", [](Scalar const& scalar) { return log10(scalar); });
                    module.def("log10", [](Value const value) { return log10(value); });

                    // multiply
                    module.def(
                        "multiply",
                        [](Array const& array1, Array const& array2) { return multiply(array1, array2); });
                    module.def(
                        "multiply",
                        [](Array const& array, Scalar const& scalar) { return multiply(array, scalar); });
                    module.def(
                        "multiply",
                        [](Array const& array, Value const value) { return multiply(array, value); });

                    module.def(
                        "multiply",
                        [](Scalar const& scalar, Array const& array) { return multiply(scalar, array); });
                    module.def(
                        "multiply",
                        [](Scalar const& scalar1, Scalar const& scalar2)
                        { return multiply(scalar1, scalar2); });
                    module.def(
                        "multiply",
                        [](Scalar const& scalar, Value const value) { return multiply(scalar, value); });

                    module.def(
                        "multiply",
                        [](Value const value, Array const& array) { return multiply(value, array); });
                    module.def(
                        "multiply",
                        [](Value const value, Scalar const& scalar) { return multiply(value, scalar); });

                    module.def(
                        "multiply",
                        [](Value const value1, Value const value2) { return multiply(value1, value2); });

                    // pow
                    module.def(
                        "pow", [](Array const& array1, Array const& array2) { return pow(array1, array2); });
                    module.def(
                        "pow", [](Array const& array, Scalar const& scalar) { return pow(array, scalar); });
                    module.def(
                        "pow", [](Array const& array, Value const value) { return pow(array, value); });

                    module.def(
                        "pow", [](Scalar const& scalar, Array const& array) { return pow(scalar, array); });
                    module.def(
                        "pow",
                        [](Scalar const& scalar1, Scalar const& scalar2) { return pow(scalar1, scalar2); });
                    module.def(
                        "pow", [](Scalar const& scalar, Value const value) { return pow(scalar, value); });

                    module.def(
                        "pow", [](Value const value, Array const& array) { return pow(value, array); });
                    module.def(
                        "pow", [](Value const value, Scalar const& scalar) { return pow(value, scalar); });

                    module.def(
                        "pow", [](Value const value1, Value const value2) { return pow(value1, value2); });

                    // sqrt
                    module.def("sqrt", [](Array const& array) { return sqrt(array); });
                    module.def("sqrt", [](Scalar const& scalar) { return sqrt(scalar); });
                    module.def("sqrt", [](Value const value) { return sqrt(value); });
                }
        };


        class SignedArithmeticElementBinder
        {

            public:

                template<Arithmetic Element>
                static void bind(pybind11::module& module)
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
        };

    }  // Anonymous namespace


    void bind_arithmetic(pybind11::module& module)
    {
        bind<ArithmeticElementBinder, ArithmeticElements>(module);
        bind<FloatingPointElementBinder, FloatingPointElements>(module);
        bind<SignedArithmeticElementBinder, SignedArithmeticElements>(module);
    }

}  // namespace lue::framework
