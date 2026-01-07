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

        template<typename Element, Rank rank>
        static auto formal_string_representation(PartitionedArray<Element, rank> const& array) -> std::string
        {
            return std::format(
                "PartitionedArray<{}, {}>{{shape={}, shape_in_partitions={}}}",
                as_string<Element>(),
                rank,
                as_string(array.shape()),
                as_string(array.partitions().shape()));
        }


        template<typename Element, Rank rank>
        static auto informal_string_representation(PartitionedArray<Element, rank> const& array)
            -> std::string
        {
            return formal_string_representation(array);
        }

    }  // namespace detail


    template<typename Element, Rank rank>
    void bind_partitioned_array(pybind11::module& module)
    {
        // https://docs.python.org/3/reference/datamodel.html
        using namespace lue::value_policies;

        using Array = PartitionedArray<Element, rank>;
        using Scalar = Scalar<Element>;
        using BooleanElement = std::uint8_t;

        auto class_ =
            pybind11::class_<Array>(
                module,
                std::format("PartitionedArray<{}, {}>", as_string<Element>(), rank).c_str(),
                std::format(
                    R"(
    Partitioned array type for arrays of rank {}, containing array
    elements of type {}
)",
                    rank,
                    as_string<Element>())
                    .c_str())

                .def_property_readonly(
                    "dtype",
                    []([[maybe_unused]] Array const& self) -> auto { return dtype<Element>(); },
                    "dtype docstring...")

                .def_property_readonly(
                    "shape",
                    [](Array const& self) -> auto
                    {
                        static_assert(rank == 2);

                        auto shape{self.shape()};

                        return std::make_tuple(shape[0], shape[1]);
                    },
                    "shape docstring...")

                .def_property_readonly(
                    "shape_in_partitions",
                    [](Array const& self) -> auto
                    {
                        static_assert(rank == 2);

                        auto shape{self.partitions().shape()};

                        return std::make_tuple(shape[0], shape[1]);
                    },
                    "shape_in_partitions docstring...")

                .def(
                    "future",
                    [](Array const& array) -> hpx::shared_future<void>
                    {
                        return hpx::when_all(array.partitions().begin(), array.partitions().end())
                            .then([](auto&&) -> auto { return hpx::make_ready_future<void>(); });
                    })

                .def(
                    "__repr__",
                    [](Array const& array) -> auto { return detail::formal_string_representation(array); })

                .def(
                    "__str__",
                    [](Array const& array) -> auto { return detail::informal_string_representation(array); })

                // bool(a), not a, if a, while a, ...
                .def(
                    "__bool__",
                    []([[maybe_unused]] Array const& argument) -> auto
                    {
                        // ValueError
                        throw std::invalid_argument("The truth value of an array is ambiguous");
                    });

        class_
            // a < b
            .def(
                "__lt__",
                [](Array const& argument1, Array const& argument2) -> auto
                { return less_than<BooleanElement>(argument1, argument2); },
                pybind11::is_operator())
            .def(
                "__lt__",
                [](Array const& argument1, Scalar const& argument2) -> auto
                { return less_than<BooleanElement>(argument1, argument2); },
                pybind11::is_operator())
            .def(
                "__lt__",
                [](Array const& argument1, Element const argument2) -> auto
                { return less_than<BooleanElement>(argument1, argument2); },
                pybind11::is_operator())

            // a <= b
            .def(
                "__le__",
                [](Array const& argument1, Array const& argument2) -> auto
                { return less_than_equal_to<BooleanElement>(argument1, argument2); },
                pybind11::is_operator())
            .def(
                "__le__",
                [](Array const& argument1, Scalar const& argument2) -> auto
                { return less_than_equal_to<BooleanElement>(argument1, argument2); },
                pybind11::is_operator())
            .def(
                "__le__",
                [](Array const& argument1, Element const argument2) -> auto
                { return less_than_equal_to<BooleanElement>(argument1, argument2); },
                pybind11::is_operator())

            // a == b
            .def(
                "__eq__",
                [](Array const& argument1, Array const& argument2) -> auto
                { return equal_to<BooleanElement>(argument1, argument2); },
                pybind11::is_operator())
            .def(
                "__eq__",
                [](Array const& argument1, Scalar const& argument2) -> auto
                { return equal_to<BooleanElement>(argument1, argument2); },
                pybind11::is_operator())
            .def(
                "__eq__",
                [](Array const& argument1, Element const argument2) -> auto
                { return equal_to<BooleanElement>(argument1, argument2); },
                pybind11::is_operator())

            // a != b
            .def(
                "__ne__",
                [](Array const& argument1, Array const& argument2) -> auto
                { return not_equal_to<BooleanElement>(argument1, argument2); },
                pybind11::is_operator())
            .def(
                "__ne__",
                [](Array const& argument1, Scalar const& argument2) -> auto
                { return not_equal_to<BooleanElement>(argument1, argument2); },
                pybind11::is_operator())
            .def(
                "__ne__",
                [](Array const& argument1, Element const argument2) -> auto
                { return not_equal_to<BooleanElement>(argument1, argument2); },
                pybind11::is_operator())

            // a > b
            .def(
                "__gt__",
                [](Array const& argument1, Array const& argument2) -> auto
                { return greater_than<BooleanElement>(argument1, argument2); },
                pybind11::is_operator())
            .def(
                "__gt__",
                [](Array const& argument1, Scalar const& argument2) -> auto
                { return greater_than<BooleanElement>(argument1, argument2); },
                pybind11::is_operator())
            .def(
                "__gt__",
                [](Array const& argument1, Element const argument2) -> auto
                { return greater_than<BooleanElement>(argument1, argument2); },
                pybind11::is_operator())

            // a >= b
            .def(
                "__ge__",
                [](Array const& argument1, Array const& argument2) -> auto
                { return greater_than_equal_to<BooleanElement>(argument1, argument2); },
                pybind11::is_operator())
            .def(
                "__ge__",
                [](Array const& argument1, Scalar const& argument2) -> auto
                { return greater_than_equal_to<BooleanElement>(argument1, argument2); },
                pybind11::is_operator())
            .def(
                "__ge__",
                [](Array const& argument1, Element const argument2) -> auto
                { return greater_than_equal_to<BooleanElement>(argument1, argument2); },
                pybind11::is_operator())

            // a + b, b + a, a += b
            .def(
                "__add__",
                [](Array const& argument1, Array const& argument2) -> auto
                { return add(argument1, argument2); },
                pybind11::is_operator())
            .def(
                "__add__",
                [](Array const& argument1, Scalar const& argument2) -> auto
                { return add(argument1, argument2); },
                pybind11::is_operator())
            .def(
                "__add__",
                [](Array const& argument1, Element const argument2) -> auto
                { return add(argument1, argument2); },
                pybind11::is_operator())

            .def(
                "__radd__",
                [](Array const& argument2, Scalar const& argument1) -> auto
                { return add(argument1, argument2); },
                pybind11::is_operator())
            .def(
                "__radd__",
                [](Array const& argument2, Element const argument1) -> auto
                { return add(argument1, argument2); },
                pybind11::is_operator())

            // a - b, b - a, a -= b
            .def(
                "__sub__",
                [](Array const& argument1, Array const& argument2) -> auto
                { return subtract(argument1, argument2); },
                pybind11::is_operator())
            .def(
                "__sub__",
                [](Array const& argument1, Scalar const& argument2) -> auto
                { return subtract(argument1, argument2); },
                pybind11::is_operator())
            .def(
                "__sub__",
                [](Array const& argument1, Element const argument2) -> auto
                { return subtract(argument1, argument2); },
                pybind11::is_operator())

            .def(
                "__rsub__",
                [](Array const& argument2, Element const argument1) -> auto
                { return subtract(argument1, argument2); },
                pybind11::is_operator())
            .def(
                "__rsub__",
                [](Array const& argument2, Scalar const& argument1) -> auto
                { return subtract(argument1, argument2); },
                pybind11::is_operator());

        if constexpr (std::is_arithmetic_v<Element>)
        {
            class_

                // a % b
                .def(
                    "__mod__",
                    [](Array const& argument1, Array const& argument2) -> auto
                    { return modulus(argument1, argument2); },
                    pybind11::is_operator())
                .def(
                    "__mod__",
                    [](Array const& argument1, Scalar const& argument2) -> auto
                    { return modulus(argument1, argument2); },
                    pybind11::is_operator())
                .def(
                    "__mod__",
                    [](Array const& argument1, Element const argument2) -> auto
                    { return modulus(argument1, argument2); },
                    pybind11::is_operator())

                .def(
                    "__rmod__",
                    [](Array const& argument2, Scalar const& argument1) -> auto
                    { return modulus(argument1, argument2); },
                    pybind11::is_operator())
                .def(
                    "__rmod__",
                    [](Array const& argument2, Element const argument1) -> auto
                    { return modulus(argument1, argument2); },
                    pybind11::is_operator())

                // a / b, b / a, a /= b
                .def(
                    "__truediv__",
                    [](Array const& argument1, Array const& argument2) -> auto
                    { return divide(argument1, argument2); },
                    pybind11::is_operator())
                .def(
                    "__truediv__",
                    [](Array const& argument1, Scalar const& argument2) -> auto
                    { return divide(argument1, argument2); },
                    pybind11::is_operator())
                .def(
                    "__truediv__",
                    [](Array const& argument1, Element const argument2) -> auto
                    { return divide(argument1, argument2); },
                    pybind11::is_operator())

                .def(
                    "__rtruediv__",
                    [](Array const& argument2, Scalar const& argument1) -> auto
                    { return divide(argument1, argument2); },
                    pybind11::is_operator())
                .def(
                    "__rtruediv__",
                    [](Array const& argument2, Element const argument1) -> auto
                    { return divide(argument1, argument2); },
                    pybind11::is_operator());
        }

        if constexpr (std::is_signed_v<Element> || std::is_floating_point_v<Element>)
        {
            class_

                // abs(a)
                .def(
                    "__abs__", [](Array const& self) -> auto { return abs(self); }, pybind11::is_operator())

                // -a
                .def(
                    "__neg__",
                    [](Array const& self) -> auto { return negate(self); },
                    pybind11::is_operator());
        }

        if constexpr (std::is_integral_v<Element>)
        {
            class_

                // a | b
                .def(
                    "__or__",
                    [](Array const& argument1, Array const& argument2) -> auto
                    { return logical_inclusive_or<BooleanElement>(argument1, argument2); },
                    pybind11::is_operator())
                .def(
                    "__or__",
                    [](Array const& argument1, Scalar const& argument2) -> auto
                    { return logical_inclusive_or<BooleanElement>(argument1, argument2); },
                    pybind11::is_operator())
                .def(
                    "__or__",
                    [](Array const& argument1, Element const& argument2) -> auto
                    { return logical_inclusive_or<BooleanElement>(argument1, argument2); },
                    pybind11::is_operator())

                .def(
                    "__ror__",
                    [](Array const& argument1, Scalar const& argument2) -> auto
                    { return logical_inclusive_or<BooleanElement>(argument1, argument2); },
                    pybind11::is_operator())
                .def(
                    "__ror__",
                    [](Array const& argument1, Element const& argument2) -> auto
                    { return logical_inclusive_or<BooleanElement>(argument1, argument2); },
                    pybind11::is_operator())

                // a ^ b
                .def(
                    "__xor__",
                    [](Array const& argument1, Array const& argument2) -> auto
                    { return logical_exclusive_or<BooleanElement>(argument1, argument2); },
                    pybind11::is_operator())
                .def(
                    "__xor__",
                    [](Array const& argument1, Scalar const& argument2) -> auto
                    { return logical_exclusive_or<BooleanElement>(argument1, argument2); },
                    pybind11::is_operator())
                .def(
                    "__xor__",
                    [](Array const& argument1, Element const& argument2) -> auto
                    { return logical_exclusive_or<BooleanElement>(argument1, argument2); },
                    pybind11::is_operator())

                .def(
                    "__rxor__",
                    [](Array const& argument1, Scalar const& argument2) -> auto
                    { return logical_exclusive_or<BooleanElement>(argument1, argument2); },
                    pybind11::is_operator())
                .def(
                    "__rxor__",
                    [](Array const& argument1, Element const& argument2) -> auto
                    { return logical_exclusive_or<BooleanElement>(argument1, argument2); },
                    pybind11::is_operator())

                // a & b
                .def(
                    "__and__",
                    [](Array const& argument1, Array const& argument2) -> auto
                    { return logical_and<BooleanElement>(argument1, argument2); },
                    pybind11::is_operator())
                .def(
                    "__and__",
                    [](Array const& argument1, Scalar const& argument2) -> auto
                    { return logical_and<BooleanElement>(argument1, argument2); },
                    pybind11::is_operator())
                .def(
                    "__and__",
                    [](Array const& argument1, Element const& argument2) -> auto
                    { return logical_and<BooleanElement>(argument1, argument2); },
                    pybind11::is_operator())

                .def(
                    "__rand__",
                    [](Array const& argument1, Scalar const& argument2) -> auto
                    { return logical_and<BooleanElement>(argument1, argument2); },
                    pybind11::is_operator())
                .def(
                    "__rand__",
                    [](Array const& argument1, Element const& argument2) -> auto
                    { return logical_and<BooleanElement>(argument1, argument2); },
                    pybind11::is_operator())

                // ~a
                .def(
                    "__invert__",
                    [](Array const& self) -> auto { return logical_not<BooleanElement>(self); },
                    pybind11::is_operator());
        }

        if constexpr (std::is_floating_point_v<Element>)
        {
            class_
                // a * b, b * a, a *= b
                .def(
                    "__mul__",
                    [](Array const& argument1, Array const& argument2) -> auto
                    { return multiply(argument1, argument2); },
                    pybind11::is_operator())
                .def(
                    "__mul__",
                    [](Array const& argument1, Scalar const& argument2) -> auto
                    { return multiply(argument1, argument2); },
                    pybind11::is_operator())
                .def(
                    "__mul__",
                    [](Array const& argument1, Element const argument2) -> auto
                    { return multiply(argument1, argument2); },
                    pybind11::is_operator())

                .def(
                    "__rmul__",
                    [](Array const& argument2, Scalar const& argument1) -> auto
                    { return multiply(argument1, argument2); },
                    pybind11::is_operator())
                .def(
                    "__rmul__",
                    [](Array const& argument2, Element const argument1) -> auto
                    { return multiply(argument1, argument2); },
                    pybind11::is_operator())

                // a // b, b // a, a //= b
                .def(
                    "__floordiv__",
                    [](Array const& argument1, Array const& argument2) -> auto
                    { return floor(divide(argument1, argument2)); },
                    pybind11::is_operator())
                .def(
                    "__floordiv__",
                    [](Array const& argument1, Scalar const& argument2) -> auto
                    { return floor(divide(argument1, argument2)); },
                    pybind11::is_operator())
                .def(
                    "__floordiv__",
                    [](Array const& argument1, Element const argument2) -> auto
                    { return floor(divide(argument1, argument2)); },
                    pybind11::is_operator())

                .def(
                    "__rfloordiv__",
                    [](Array const& argument2, Scalar const& argument1) -> auto
                    { return floor(divide(argument1, argument2)); },
                    pybind11::is_operator())
                .def(
                    "__rfloordiv__",
                    [](Array const& argument2, Element const argument1) -> auto
                    { return floor(divide(argument1, argument2)); },
                    pybind11::is_operator())

                // a ** b, b ** a, a **= b
                .def(
                    "__pow__",
                    [](Array const& argument1, Array const& argument2) -> auto
                    { return pow(argument1, argument2); },
                    pybind11::is_operator())
                .def(
                    "__pow__",
                    [](Array const& argument1, Scalar const& argument2) -> auto
                    { return pow(argument1, argument2); },
                    pybind11::is_operator())
                .def(
                    "__pow__",
                    [](Array const& argument1, Element const argument2) -> auto
                    { return pow(argument1, argument2); },
                    pybind11::is_operator())

                .def(
                    "__rpow__",
                    [](Array const& argument2, Scalar const& argument1) -> auto
                    { return pow(argument1, argument2); },
                    pybind11::is_operator())
                .def(
                    "__rpow__",
                    [](Array const& argument2, Element const argument1) -> auto
                    { return pow(argument1, argument2); },
                    pybind11::is_operator());
        }
    }

}  // namespace lue::framework
