#include "lue/framework/algorithm/value_policies/abs.hpp"
#include "lue/framework/algorithm/value_policies/add.hpp"
#include "lue/framework/algorithm/value_policies/divide.hpp"
#include "lue/framework/algorithm/value_policies/equal_to.hpp"
#include "lue/framework/algorithm/value_policies/floor.hpp"
#include "lue/framework/algorithm/value_policies/greater_than.hpp"
#include "lue/framework/algorithm/value_policies/greater_than_equal_to.hpp"
#include "lue/framework/algorithm/value_policies/less_than.hpp"
#include "lue/framework/algorithm/value_policies/less_than_equal_to.hpp"
#include "lue/framework/algorithm/value_policies/logical_and.hpp"
#include "lue/framework/algorithm/value_policies/logical_exclusive_or.hpp"
#include "lue/framework/algorithm/value_policies/logical_inclusive_or.hpp"
#include "lue/framework/algorithm/value_policies/logical_not.hpp"
#include "lue/framework/algorithm/value_policies/modulus.hpp"
#include "lue/framework/algorithm/value_policies/multiply.hpp"
#include "lue/framework/algorithm/value_policies/negate.hpp"
#include "lue/framework/algorithm/value_policies/not_equal_to.hpp"
#include "lue/framework/algorithm/value_policies/pow.hpp"
#include "lue/framework/algorithm/value_policies/subtract.hpp"
#include "lue/py/framework/stream.hpp"
#include <pybind11/pybind11.h>


namespace lue::framework {
    namespace detail {

        template<typename Element>
        auto formal_string_representation([[maybe_unused]] Scalar<Element> const& scalar) -> std::string
        {
            return std::format("Scalar<{}>{{}}", as_string<Element>());
        }


        template<typename Element>
        auto informal_string_representation(Scalar<Element> const& scalar) -> std::string
        {
            return formal_string_representation(scalar);
        }

    }  // namespace detail


    template<typename Element>
    void bind_scalar(pybind11::module& module)
    {
        // https://docs.python.org/3/reference/datamodel.html
        using namespace lue::value_policies;

        using Scalar = lue::Scalar<Element>;
        using BooleanElement = std::uint8_t;

        auto class_ =
            pybind11::class_<Scalar, pybind11::smart_holder>(
                module,
                std::format("Scalar<{}>", as_string<Element>()).c_str(),
                std::format(
                    R"(
    Scalar containing element of type {}
)",
                    as_string<Element>())
                    .c_str())

                // .def(pybind11::init<Element>())

                .def_property_readonly(
                    "dtype",
                    []([[maybe_unused]] Scalar const& self) -> auto { return dtype<Element>(); },
                    "dtype docstring...")

                .def_property_readonly(
                    "future",
                    [](Scalar const& self) -> hpx::shared_future<Element> { return self.future(); },
                    "future docstring...")

                .def(
                    "__repr__",
                    [](Scalar const& scalar) -> auto { return detail::formal_string_representation(scalar); })

                .def(
                    "__str__",
                    [](Scalar const& scalar) -> auto
                    { return detail::informal_string_representation(scalar); })

                // bool(a), not a, if a, while a, ...
                .def(
                    "__bool__",
                    []([[maybe_unused]] Scalar const& argument) -> auto
                    {
                        // ValueError
                        throw std::invalid_argument("The truth value of a scalar is ambiguous");
                    });

        class_
            // a < b
            .def(
                "__lt__",
                [](Scalar const& argument1, Scalar const& argument2) -> auto
                { return less_than<BooleanElement>(argument1, argument2); },
                pybind11::is_operator())
            .def(
                "__lt__",
                [](Scalar const& argument1, Element const& argument2) -> auto
                { return less_than<BooleanElement>(argument1, argument2); },
                pybind11::is_operator())

            // a <= b
            .def(
                "__le__",
                [](Scalar const& argument1, Scalar const& argument2) -> auto
                { return less_than_equal_to<BooleanElement>(argument1, argument2); },
                pybind11::is_operator())
            .def(
                "__le__",
                [](Scalar const& argument1, Element const& argument2) -> auto
                { return less_than_equal_to<BooleanElement>(argument1, argument2); },
                pybind11::is_operator())

            // a == b
            .def(
                "__eq__",
                [](Scalar const& argument1, Scalar const& argument2) -> auto
                { return equal_to<BooleanElement>(argument1, argument2); },
                pybind11::is_operator())
            .def(
                "__eq__",
                [](Scalar const& argument1, Element const& argument2) -> auto
                { return equal_to<BooleanElement>(argument1, argument2); },
                pybind11::is_operator())

            // a != b
            .def(
                "__ne__",
                [](Scalar const& argument1, Scalar const& argument2) -> auto
                { return not_equal_to<BooleanElement>(argument1, argument2); },
                pybind11::is_operator())
            .def(
                "__ne__",
                [](Scalar const& argument1, Element const& argument2) -> auto
                { return not_equal_to<BooleanElement>(argument1, argument2); },
                pybind11::is_operator())

            // a > b
            .def(
                "__gt__",
                [](Scalar const& argument1, Scalar const& argument2) -> auto
                { return greater_than<BooleanElement>(argument1, argument2); },
                pybind11::is_operator())
            .def(
                "__gt__",
                [](Scalar const& argument1, Element const& argument2) -> auto
                { return greater_than<BooleanElement>(argument1, argument2); },
                pybind11::is_operator())

            // a >= b
            .def(
                "__ge__",
                [](Scalar const& argument1, Scalar const& argument2) -> auto
                { return greater_than_equal_to<BooleanElement>(argument1, argument2); },
                pybind11::is_operator())
            .def(
                "__ge__",
                [](Scalar const& argument1, Element const& argument2) -> auto
                { return greater_than_equal_to<BooleanElement>(argument1, argument2); },
                pybind11::is_operator())

            // a + b, b + a, a += b
            .def(
                "__add__",
                [](Scalar const& argument1, Scalar const& argument2) -> auto
                { return add(argument1, argument2); },
                pybind11::is_operator())
            .def(
                "__add__",
                [](Scalar const& argument1, Element const& argument2) -> auto
                { return add(argument1, argument2); },
                pybind11::is_operator())

            .def(
                "__radd__",
                [](Scalar const& argument2, Element const& argument1) -> auto
                { return add(argument1, argument2); },
                pybind11::is_operator())

            // a - b, b - a, a -= b
            .def(
                "__sub__",
                [](Scalar const& argument1, Scalar const& argument2) -> auto
                { return subtract(argument1, argument2); },
                pybind11::is_operator())
            .def(
                "__sub__",
                [](Scalar const& argument1, Element const& argument2) -> auto
                { return subtract(argument1, argument2); },
                pybind11::is_operator())

            .def(
                "__rsub__",
                [](Scalar const& argument2, Element const& argument1) -> auto
                { return subtract(argument1, argument2); },
                pybind11::is_operator());

        if constexpr (std::is_arithmetic_v<Element>)
        {
            class_

                // a % b
                .def(
                    "__mod__",
                    [](Scalar const& argument1, Scalar const& argument2) -> auto
                    { return modulus(argument1, argument2); },
                    pybind11::is_operator())
                .def(
                    "__mod__",
                    [](Scalar const& argument1, Element const& argument2) -> auto
                    { return modulus(argument1, argument2); },
                    pybind11::is_operator())
                .def(
                    "__rmod__",
                    [](Scalar const& argument1, Element const& argument2) -> auto
                    { return modulus(argument1, argument2); },
                    pybind11::is_operator())

                // a / b, b / a, a /= b
                .def(
                    "__truediv__",
                    [](Scalar const& argument1, Scalar const& argument2) -> auto
                    { return divide(argument1, argument2); },
                    pybind11::is_operator())
                .def(
                    "__truediv__",
                    [](Scalar const& argument1, Element const& argument2) -> auto
                    { return divide(argument1, argument2); },
                    pybind11::is_operator())

                .def(
                    "__rtruediv__",
                    [](Scalar const& argument2, Element const& argument1) -> auto
                    { return divide(argument1, argument2); },
                    pybind11::is_operator());
        }

        if constexpr (std::is_signed_v<Element> || std::is_floating_point_v<Element>)
        {
            class_
                // abs(a)
                .def(
                    "__abs__",
                    [](Scalar const& argument) -> auto { return abs(argument); },
                    pybind11::is_operator())

                // -a
                .def(
                    "__neg__",
                    [](Scalar const& argument) -> auto { return negate(argument); },
                    pybind11::is_operator());
        }

        if constexpr (std::is_integral_v<Element>)
        {
            class_
                // a | b
                .def(
                    "__or__",
                    [](Scalar const& argument1, Scalar const& argument2) -> auto
                    { return logical_inclusive_or<BooleanElement>(argument1, argument2); },
                    pybind11::is_operator())
                .def(
                    "__or__",
                    [](Scalar const& argument1, Element const& argument2) -> auto
                    { return logical_inclusive_or<BooleanElement>(argument1, argument2); },
                    pybind11::is_operator())

                // a ^ b
                .def(
                    "__xor__",
                    [](Scalar const& argument1, Scalar const& argument2) -> auto
                    { return logical_exclusive_or<BooleanElement>(argument1, argument2); },
                    pybind11::is_operator())
                .def(
                    "__xor__",
                    [](Scalar const& argument1, Element const& argument2) -> auto
                    { return logical_exclusive_or<BooleanElement>(argument1, argument2); },
                    pybind11::is_operator())

                // a & b
                .def(
                    "__and__",
                    [](Scalar const& argument1, Scalar const& argument2) -> auto
                    { return logical_and<BooleanElement>(argument1, argument2); },
                    pybind11::is_operator())
                .def(
                    "__and__",
                    [](Scalar const& argument1, Element const& argument2) -> auto
                    { return logical_and<BooleanElement>(argument1, argument2); },
                    pybind11::is_operator())

                // ~a
                .def(
                    "__invert__",
                    [](Scalar const& argument) -> auto { return logical_not<BooleanElement>(argument); },
                    pybind11::is_operator());
        }

        if constexpr (std::is_floating_point_v<Element>)
        {
            class_
                // a * b, b * a, a *= b
                .def(
                    "__mul__",
                    [](Scalar const& argument1, Scalar const& argument2) -> auto
                    { return multiply(argument1, argument2); },
                    pybind11::is_operator())
                .def(
                    "__mul__",
                    [](Scalar const& argument1, Element const& argument2) -> auto
                    { return multiply(argument1, argument2); },
                    pybind11::is_operator())

                .def(
                    "__rmul__",
                    [](Scalar const& argument2, Element const& argument1) -> auto
                    { return multiply(argument1, argument2); },
                    pybind11::is_operator())

                // a // b, b // a, a //= b
                .def(
                    "__floordiv__",
                    [](Scalar const& argument1, Scalar const& argument2) -> auto
                    { return floor(divide(argument1, argument2)); },
                    pybind11::is_operator())
                .def(
                    "__floordiv__",
                    [](Scalar const& argument1, Element const& argument2) -> auto
                    { return floor(divide(argument1, argument2)); },
                    pybind11::is_operator())

                .def(
                    "__rfloordiv__",
                    [](Scalar const& argument2, Element const& argument1) -> auto
                    { return floor(divide(argument1, argument2)); },
                    pybind11::is_operator())

                // a ** b, b ** a, a **= b
                .def(
                    "__pow__",
                    [](Scalar const& argument1, Scalar const& argument2) -> auto
                    { return pow(argument1, argument2); },
                    pybind11::is_operator())
                .def(
                    "__pow__",
                    [](Scalar const& argument1, Element const& argument2) -> auto
                    { return pow(argument1, argument2); },
                    pybind11::is_operator())

                .def(
                    "__rpow__",
                    [](Scalar const& argument2, Element const& argument1) -> auto
                    { return pow(argument1, argument2); },
                    pybind11::is_operator());
        }
    }

}  // namespace lue::framework
