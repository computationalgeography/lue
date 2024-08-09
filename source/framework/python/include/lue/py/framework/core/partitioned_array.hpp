#include "lue/framework/algorithm/value_policies/abs.hpp"
#include "lue/framework/algorithm/value_policies/add.hpp"
#include "lue/framework/algorithm/value_policies/divide.hpp"
#include "lue/framework/algorithm/value_policies/equal_to.hpp"
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

    template<typename Element, Rank rank>
    static auto formal_string_representation(PartitionedArray<Element, rank> const& array) -> std::string
    {
        return fmt::format(
            "PartitionedArray<{}, {}>{{shape={}, shape_in_partitions={}}}",
            as_string<Element>(),
            rank,
            as_string(array.shape()),
            as_string(array.partitions().shape()));
    }


    template<typename Element, Rank rank>
    static auto informal_string_representation(PartitionedArray<Element, rank> const& array) -> std::string
    {
        return formal_string_representation(array);
    }


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
                fmt::format("PartitionedArray<{}, {}>", as_string<Element>(), rank).c_str(),
                fmt::format(
                    R"(
    Partitioned array type for arrays of rank {}, containing array
    elements of type {}
)",
                    rank,
                    as_string<Element>())
                    .c_str())

                .def_property_readonly(
                    "dtype",
                    []([[maybe_unused]] Array const& self) { return dtype<Element>(); },
                    "dtype docstring...")

                .def_property_readonly(
                    "shape",
                    [](Array const& self)
                    {
                        static_assert(rank == 2);

                        auto shape{self.shape()};

                        return std::make_tuple(shape[0], shape[1]);
                    },
                    "shape docstring...")

                .def_property_readonly(
                    "shape_in_partitions",
                    [](Array const& self)
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
                            .then([](auto&&) { return hpx::make_ready_future<void>(); });
                    })

                .def("__repr__", [](Array const& array) { return formal_string_representation(array); })

                .def("__str__", [](Array const& array) { return informal_string_representation(array); })

                // bool(a), not a, if a, while a, ...
                .def(
                    "__bool__",
                    []([[maybe_unused]] Array const& argument)
                    {
                        // ValueError
                        throw std::invalid_argument("The truth value of an array is ambiguous");
                    })

                // a < b
                .def(
                    "__lt__",
                    [](Array const& argument1, Array const& argument2)
                    { return less_than<BooleanElement>(argument1, argument2); },
                    pybind11::is_operator())
                .def(
                    "__lt__",
                    [](Array const& argument1, Element const argument2)
                    { return less_than<BooleanElement>(argument1, argument2); },
                    pybind11::is_operator())
                .def(
                    "__lt__",
                    [](Array const& argument1, Scalar const& argument2)
                    { return less_than<BooleanElement>(argument1, argument2); },
                    pybind11::is_operator())

                // a <= b
                .def(
                    "__le__",
                    [](Array const& argument1, Array const& argument2)
                    { return less_than_equal_to<BooleanElement>(argument1, argument2); },
                    pybind11::is_operator())
                .def(
                    "__le__",
                    [](Array const& argument1, Element const argument2)
                    { return less_than_equal_to<BooleanElement>(argument1, argument2); },
                    pybind11::is_operator())
                .def(
                    "__le__",
                    [](Array const& argument1, Scalar const& argument2)
                    { return less_than_equal_to<BooleanElement>(argument1, argument2); },
                    pybind11::is_operator())

                // a == b
                .def(
                    "__eq__",
                    [](Array const& argument1, Array const& argument2)
                    { return equal_to<BooleanElement>(argument1, argument2); },
                    pybind11::is_operator())
                .def(
                    "__eq__",
                    [](Array const& argument1, Element const argument2)
                    { return equal_to<BooleanElement>(argument1, argument2); },
                    pybind11::is_operator())
                .def(
                    "__eq__",
                    [](Array const& argument1, Scalar const& argument2)
                    { return equal_to<BooleanElement>(argument1, argument2); },
                    pybind11::is_operator())

                // a != b
                .def(
                    "__ne__",
                    [](Array const& argument1, Array const& argument2)
                    { return not_equal_to<BooleanElement>(argument1, argument2); },
                    pybind11::is_operator())
                .def(
                    "__ne__",
                    [](Array const& argument1, Element const argument2)
                    { return not_equal_to<BooleanElement>(argument1, argument2); },
                    pybind11::is_operator())
                .def(
                    "__ne__",
                    [](Array const& argument1, Scalar const& argument2)
                    { return not_equal_to<BooleanElement>(argument1, argument2); },
                    pybind11::is_operator())

                // a > b
                .def(
                    "__gt__",
                    [](Array const& argument1, Array const& argument2)
                    { return greater_than<BooleanElement>(argument1, argument2); },
                    pybind11::is_operator())
                .def(
                    "__gt__",
                    [](Array const& argument1, Element const argument2)
                    { return greater_than<BooleanElement>(argument1, argument2); },
                    pybind11::is_operator())
                .def(
                    "__gt__",
                    [](Array const& argument1, Scalar const& argument2)
                    { return greater_than<BooleanElement>(argument1, argument2); },
                    pybind11::is_operator())

                // a >= b
                .def(
                    "__ge__",
                    [](Array const& argument1, Array const& argument2)
                    { return greater_than_equal_to<BooleanElement>(argument1, argument2); },
                    pybind11::is_operator())
                .def(
                    "__ge__",
                    [](Array const& argument1, Element const argument2)
                    { return greater_than_equal_to<BooleanElement>(argument1, argument2); },
                    pybind11::is_operator())
                .def(
                    "__ge__",
                    [](Array const& argument1, Scalar const& argument2)
                    { return greater_than_equal_to<BooleanElement>(argument1, argument2); },
                    pybind11::is_operator())

            ;


        // a + b, b + a, a += b
        class_
            .def(
                "__add__",
                [](Array const& argument1, Array const& argument2) { return add(argument1, argument2); },
                pybind11::is_operator())
            .def(
                "__add__",
                [](Array const& argument1, Element const argument2) { return add(argument1, argument2); },
                pybind11::is_operator())
            .def(
                "__add__",
                [](Array const& argument1, Scalar const& argument2) { return add(argument1, argument2); },
                pybind11::is_operator())
            .def(
                "__radd__",
                [](Array const& argument2, Element const argument1) { return add(argument1, argument2); },
                pybind11::is_operator())
            .def(
                "__radd__",
                [](Array const& argument2, Scalar const& argument1) { return add(argument1, argument2); },
                pybind11::is_operator());

        // a - b, b - a, a -= b
        class_
            .def(
                "__sub__",
                [](Array const& argument1, Array const& argument2) { return subtract(argument1, argument2); },
                pybind11::is_operator())
            .def(
                "__sub__",
                [](Array const& argument1, Element const argument2)
                { return subtract(argument1, argument2); },
                pybind11::is_operator())
            .def(
                "__sub__",
                [](Array const& argument1, Scalar const& argument2)
                { return subtract(argument1, argument2); },
                pybind11::is_operator())
            .def(
                "__rsub__",
                [](Array const& argument2, Element const argument1)
                { return subtract(argument1, argument2); },
                pybind11::is_operator())
            .def(
                "__rsub__",
                [](Array const& argument2, Scalar const& argument1)
                { return subtract(argument1, argument2); },
                pybind11::is_operator());

        if constexpr (std::is_signed_v<Element> || std::is_floating_point_v<Element>)
        {
            // abs(a)
            class_.def(
                "__abs__", [](Array const& self) { return abs(self); }, pybind11::is_operator());

            // -a
            class_.def(
                "__neg__", [](Array const& self) { return negate(self); }, pybind11::is_operator());
        }

        if constexpr (std::is_integral_v<Element>)
        {
            // a | b
            class_.def(
                "__or__",
                [](Array const& argument1, Array const& argument2)
                { return logical_inclusive_or<BooleanElement>(argument1, argument2); },
                pybind11::is_operator());

            // a ^ b
            class_.def(
                "__xor__",
                [](Array const& argument1, Array const& argument2)
                { return logical_exclusive_or<BooleanElement>(argument1, argument2); },
                pybind11::is_operator());

            // a & b
            class_.def(
                "__and__",
                [](Array const& argument1, Array const& argument2)
                { return logical_and<BooleanElement>(argument1, argument2); },
                pybind11::is_operator());

            // ~a
            class_.def(
                "__invert__",
                [](Array const& self) { return logical_not<BooleanElement>(self); },
                pybind11::is_operator());

            // a % b
            class_
                .def(
                    "__mod__",
                    [](Array const& argument1, Array const& argument2)
                    { return modulus(argument1, argument2); },
                    pybind11::is_operator())
                .def(
                    "__mod__",
                    [](Array const& argument1, Element const argument2)
                    { return modulus(argument1, argument2); },
                    pybind11::is_operator())
                .def(
                    "__mod__",
                    [](Array const& argument1, Scalar const& argument2)
                    { return modulus(argument1, argument2); },
                    pybind11::is_operator())
                .def(
                    "__rmod__",
                    [](Array const& argument2, Element const argument1)
                    { return modulus(argument1, argument2); },
                    pybind11::is_operator())
                .def(
                    "__rmod__",
                    [](Array const& argument2, Scalar const& argument1)
                    { return modulus(argument1, argument2); },
                    pybind11::is_operator());
        }

        if constexpr (std::is_floating_point_v<Element>)
        {
            // a * b, b * a, a *= b
            class_
                .def(
                    "__mul__",
                    [](Array const& argument1, Array const& argument2)
                    { return multiply(argument1, argument2); },
                    pybind11::is_operator())
                .def(
                    "__mul__",
                    [](Array const& argument1, Element const argument2)
                    { return multiply(argument1, argument2); },
                    pybind11::is_operator())
                .def(
                    "__mul__",
                    [](Array const& argument1, Scalar const& argument2)
                    { return multiply(argument1, argument2); },
                    pybind11::is_operator())
                .def(
                    "__rmul__",
                    [](Array const& argument2, Element const argument1)
                    { return multiply(argument1, argument2); },
                    pybind11::is_operator())
                .def(
                    "__rmul__",
                    [](Array const& argument2, Scalar const& argument1)
                    { return multiply(argument1, argument2); },
                    pybind11::is_operator());

            // a / b, b / a, a /= b
            class_
                .def(
                    "__truediv__",
                    [](Array const& argument1, Array const& argument2)
                    { return divide(argument1, argument2); },
                    pybind11::is_operator())
                .def(
                    "__truediv__",
                    [](Array const& argument1, Element const argument2)
                    { return divide(argument1, argument2); },
                    pybind11::is_operator())
                .def(
                    "__truediv__",
                    [](Array const& argument1, Scalar const& argument2)
                    { return divide(argument1, argument2); },
                    pybind11::is_operator())
                .def(
                    "__rtruediv__",
                    [](Array const& argument2, Element const argument1)
                    { return divide(argument1, argument2); },
                    pybind11::is_operator())
                .def(
                    "__rtruediv__",
                    [](Array const& argument2, Scalar const& argument1)
                    { return divide(argument1, argument2); },
                    pybind11::is_operator());

            // a ** b, b ** a, a **= b
            class_
                .def(
                    "__pow__",
                    [](Array const& argument1, Array const& argument2) { return pow(argument1, argument2); },
                    pybind11::is_operator())
                .def(
                    "__pow__",
                    [](Array const& argument1, Element const argument2) { return pow(argument1, argument2); },
                    pybind11::is_operator())
                .def(
                    "__pow__",
                    [](Array const& argument1, Scalar const& argument2) { return pow(argument1, argument2); },
                    pybind11::is_operator())
                .def(
                    "__rpow__",
                    [](Array const& argument2, Element const argument1) { return pow(argument1, argument2); },
                    pybind11::is_operator())
                .def(
                    "__rpow__",
                    [](Array const& argument2, Scalar const& argument1) { return pow(argument1, argument2); },
                    pybind11::is_operator());
        }
    }

}  // namespace lue::framework
