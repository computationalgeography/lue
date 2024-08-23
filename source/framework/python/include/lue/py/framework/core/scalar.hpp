// #include "lue/py/framework/algorithm/abs.hpp"
#include "lue/framework/algorithm/value_policies/add.hpp"
#include "lue/framework/algorithm/value_policies/divide.hpp"
// #include "lue/py/framework/algorithm/equal_to.hpp"
// #include "lue/py/framework/algorithm/greater_than.hpp"
// #include "lue/py/framework/algorithm/greater_than_equal_to.hpp"
// #include "lue/py/framework/algorithm/less_than.hpp"
// #include "lue/py/framework/algorithm/less_than_equal_to.hpp"
// #include "lue/py/framework/algorithm/logical_and.hpp"
// #include "lue/py/framework/algorithm/logical_exclusive_or.hpp"
// #include "lue/py/framework/algorithm/logical_inclusive_or.hpp"
// #include "lue/py/framework/algorithm/logical_not.hpp"
#include "lue/framework/algorithm/value_policies/multiply.hpp"
// #include "lue/py/framework/algorithm/negate.hpp"
// #include "lue/py/framework/algorithm/not_equal_to.hpp"
#include "lue/framework/algorithm/value_policies/pow.hpp"
#include "lue/framework/algorithm/value_policies/subtract.hpp"
#include "lue/py/framework/stream.hpp"
#include <pybind11/pybind11.h>


using namespace pybind11::literals;


namespace lue::framework {

    namespace {

        template<typename Element>
        static std::string formal_string_representation([[maybe_unused]] Scalar<Element> const& scalar)
        {
            return fmt::format("Scalar<{}>{{}}", as_string<Element>());
        }


        template<typename Element>
        static std::string informal_string_representation(Scalar<Element> const& scalar)
        {
            return formal_string_representation(scalar);
        }


        template<typename Element>
        auto create_scalar(pybind11::object const& value) -> Scalar<Element>
        {
            return pybind11::cast<Element>(value);
        }


        auto create_scalar_py(pybind11::object const& dtype_args, pybind11::object const& value)
            -> pybind11::object
        {
            pybind11::dtype const dtype{pybind11::dtype::from_args(dtype_args)};

            // Switch on dtype and return a scalar of the right type

            auto const kind = dtype.kind();
            auto const size = dtype.itemsize();  // bytes
            pybind11::object result;

            switch (kind)
            {
                case 'i':
                {
                    // Signed integer
                    switch (size)
                    {
                        case 4:
                        {
                            result = pybind11::cast(create_scalar<std::int32_t>(value));
                            break;
                        }
                        case 8:
                        {
                            result = pybind11::cast(create_scalar<std::int64_t>(value));
                            break;
                        }
                    }

                    break;
                }
                case 'u':
                {
                    // Unsigned integer
                    switch (size)
                    {
                        case 1:
                        {
                            result = pybind11::cast(create_scalar<std::uint8_t>(value));
                            break;
                        }
                        case 4:
                        {
                            result = pybind11::cast(create_scalar<std::uint32_t>(value));
                            break;
                        }
                        case 8:
                        {
                            result = pybind11::cast(create_scalar<std::uint64_t>(value));
                            break;
                        }
                    }

                    break;
                }
                case 'f':
                {
                    // Floating-point
                    switch (size)
                    {
                        case 4:
                        {
                            result = pybind11::cast(create_scalar<float>(value));
                            break;
                        }
                        case 8:
                        {
                            result = pybind11::cast(create_scalar<double>(value));
                            break;
                        }
                    }

                    break;
                }
            }

            if (!result)
            {
                throw std::runtime_error(fmt::format("Unsupported dtype (kind={}, itemsize={})", kind, size));
            }

            return result;
        }

    }  // Anonymous namespace


    template<typename Element>
    void bind_scalar(pybind11::module& module)
    {
        using namespace lue::value_policies;
        using Scalar = lue::Scalar<Element>;
        using Value = Element;

        auto class_ =
            pybind11::class_<Scalar>(
                module,
                fmt::format("Scalar<{}>", as_string<Element>()).c_str(),
                fmt::format(
                    R"(
    Scalar containing element of type {}
)",
                    as_string<Element>())
                    .c_str())

                // .def(pybind11::init<Element>())

                .def_property_readonly(
                    "dtype",
                    []([[maybe_unused]] Scalar const& self) { return dtype<Element>(); },
                    "dtype docstring...")

                .def_property_readonly(
                    "future",
                    [](Scalar const& self) -> hpx::shared_future<Element> { return self.future(); },
                    "future docstring...")

                .def("__repr__", [](Scalar const& scalar) { return formal_string_representation(scalar); })

                .def("__str__", [](Scalar const& scalar) { return informal_string_representation(scalar); })

                // bool(a), not a, if a, while a, ...
                .def(
                    "__bool__",
                    []([[maybe_unused]] Scalar const& argument)
                    {
                        // ValueError
                        throw std::invalid_argument("The truth value of a scalar is ambiguous");
                    })

            //                 // a < b
            //                 .def(
            //                     "__lt__",
            //                     [](Scalar const& argument1, Scalar const& argument2)
            //                     { return lfr::less_than<ElementT<Scalar>, std::uint8_t>(argument1,
            //                     argument2); }, pybind11::is_operator())
            //                 .def(
            //                     "__lt__",
            //                     [](Scalar const& argument1, Element const argument2)
            //                     { return lfr::less_than<ElementT<Scalar>, std::uint8_t>(argument1,
            //                     argument2); }, pybind11::is_operator())
            //                 .def(
            //                     "__lt__",
            //                     [](Scalar const& argument1, Value const& argument2)
            //                     { return lfr::less_than<ElementT<Scalar>, std::uint8_t>(argument1,
            //                     argument2); }, pybind11::is_operator())
            //
            //                 // a <= b
            //                 .def(
            //                     "__le__",
            //                     [](Scalar const& argument1, Scalar const& argument2) {
            //                         return lfr::less_than_equal_to<ElementT<Scalar>, std::uint8_t>(
            //                             argument1, argument2);
            //                     },
            //                     pybind11::is_operator())
            //                 .def(
            //                     "__le__",
            //                     [](Scalar const& argument1, Element const argument2) {
            //                         return lfr::less_than_equal_to<ElementT<Scalar>, std::uint8_t>(
            //                             argument1, argument2);
            //                     },
            //                     pybind11::is_operator())
            //                 .def(
            //                     "__le__",
            //                     [](Scalar const& argument1, Value const& argument2) {
            //                         return lfr::less_than_equal_to<ElementT<Scalar>, std::uint8_t>(
            //                             argument1, argument2);
            //                     },
            //                     pybind11::is_operator())
            //
            //                 // a == b
            //                 .def(
            //                     "__eq__",
            //                     [](Scalar const& argument1, Scalar const& argument2)
            //                     { return lfr::equal_to<ElementT<Scalar>, std::uint8_t>(argument1,
            //                     argument2); }, pybind11::is_operator())
            //                 .def(
            //                     "__eq__",
            //                     [](Scalar const& argument1, Element const argument2)
            //                     { return lfr::equal_to<ElementT<Scalar>, std::uint8_t>(argument1,
            //                     argument2); }, pybind11::is_operator())
            //                 .def(
            //                     "__eq__",
            //                     [](Scalar const& argument1, Value const& argument2)
            //                     { return lfr::equal_to<ElementT<Scalar>, std::uint8_t>(argument1,
            //                     argument2); }, pybind11::is_operator())
            //
            //                 // a != b
            //                 .def(
            //                     "__ne__",
            //                     [](Scalar const& argument1, Scalar const& argument2)
            //                     { return lfr::not_equal_to<ElementT<Scalar>, std::uint8_t>(argument1,
            //                     argument2); }, pybind11::is_operator())
            //                 .def(
            //                     "__ne__",
            //                     [](Scalar const& argument1, Element const argument2)
            //                     { return lfr::not_equal_to<ElementT<Scalar>, std::uint8_t>(argument1,
            //                     argument2); }, pybind11::is_operator())
            //                 .def(
            //                     "__ne__",
            //                     [](Scalar const& argument1, Value const& argument2)
            //                     { return lfr::not_equal_to<ElementT<Scalar>, std::uint8_t>(argument1,
            //                     argument2); }, pybind11::is_operator())
            //
            //                 // a > b
            //                 .def(
            //                     "__gt__",
            //                     [](Scalar const& argument1, Scalar const& argument2)
            //                     { return lfr::greater_than<ElementT<Scalar>, std::uint8_t>(argument1,
            //                     argument2); }, pybind11::is_operator())
            //                 .def(
            //                     "__gt__",
            //                     [](Scalar const& argument1, Element const argument2)
            //                     { return lfr::greater_than<ElementT<Scalar>, std::uint8_t>(argument1,
            //                     argument2); }, pybind11::is_operator())
            //                 .def(
            //                     "__gt__",
            //                     [](Scalar const& argument1, Value const& argument2)
            //                     { return lfr::greater_than<ElementT<Scalar>, std::uint8_t>(argument1,
            //                     argument2); }, pybind11::is_operator())
            //
            //                 // a >= b
            //                 .def(
            //                     "__ge__",
            //                     [](Scalar const& argument1, Scalar const& argument2) {
            //                         return lfr::greater_than_equal_to<ElementT<Scalar>, std::uint8_t>(
            //                             argument1, argument2);
            //                     },
            //                     pybind11::is_operator())
            //                 .def(
            //                     "__ge__",
            //                     [](Scalar const& argument1, Element const argument2) {
            //                         return lfr::greater_than_equal_to<ElementT<Scalar>, std::uint8_t>(
            //                             argument1, argument2);
            //                     },
            //                     pybind11::is_operator())
            //                 .def(
            //                     "__ge__",
            //                     [](Scalar const& argument1, Value const& argument2) {
            //                         return lfr::greater_than_equal_to<ElementT<Scalar>, std::uint8_t>(
            //                             argument1, argument2);
            //                     },
            //                     pybind11::is_operator())

            ;


        // a + b, b + a, a += b
        class_
            .def(
                "__add__",
                [](Scalar const& argument1, Scalar const& argument2) { return add(argument1, argument2); },
                pybind11::is_operator())
            .def(
                "__add__",
                [](Scalar const& argument1, Value const& argument2) { return add(argument1, argument2); },
                pybind11::is_operator())
            .def(
                "__radd__",
                [](Scalar const& argument2, Value const& argument1) { return add(argument1, argument2); },
                pybind11::is_operator());

        // a - b, b - a, a -= b
        class_
            .def(
                "__sub__",
                [](Scalar const& argument1, Scalar const& argument2)
                { return subtract(argument1, argument2); },
                pybind11::is_operator())
            .def(
                "__sub__",
                [](Scalar const& argument1, Element const argument2)
                { return subtract(argument1, argument2); },
                pybind11::is_operator())
            .def(
                "__sub__",
                [](Scalar const& argument1, Value const& argument2)
                { return subtract(argument1, argument2); },
                pybind11::is_operator())
            .def(
                "__rsub__",
                [](Scalar const& argument2, Element const argument1)
                { return subtract(argument1, argument2); },
                pybind11::is_operator())
            .def(
                "__rsub__",
                [](Scalar const& argument2, Value const& argument1)
                { return subtract(argument1, argument2); },
                pybind11::is_operator());


        //         if constexpr (std::is_signed_v<Element> || std::is_floating_point_v<Element>)
        //         {
        //             // abs(a)
        //             class_.def(
        //                 "__abs__",
        //                 [](Scalar const& argument) { return lfr::abs<ElementT<Scalar>>(argument); },
        //                 pybind11::is_operator());
        //
        //             // -a
        //             class_.def(
        //                 "__neg__",
        //                 [](Scalar const& argument) { return lfr::negate<ElementT<Scalar>>(argument); },
        //                 pybind11::is_operator());
        //         }
        //
        //         if constexpr (std::is_integral_v<Element>)
        //         {
        //             // a | b
        //             class_.def(
        //                 "__or__",
        //                 [](Scalar const& argument1, Scalar const& argument2)
        //                 { return lfr::logical_inclusive_or<ElementT<Scalar>>(argument1, argument2); },
        //                 pybind11::is_operator());
        //
        //             // a ^ b
        //             class_.def(
        //                 "__xor__",
        //                 [](Scalar const& argument1, Scalar const& argument2)
        //                 { return lfr::logical_exclusive_or<ElementT<Scalar>>(argument1, argument2); },
        //                 pybind11::is_operator());
        //
        //             // a & b
        //             class_.def(
        //                 "__and__",
        //                 [](Scalar const& argument1, Scalar const& argument2)
        //                 { return lfr::logical_and<ElementT<Scalar>>(argument1, argument2); },
        //                 pybind11::is_operator());
        //
        //             // ~a
        //             class_.def(
        //                 "__invert__",
        //                 [](Scalar const& argument) { return lfr::logical_not<ElementT<Scalar>>(argument);
        //                 }, pybind11::is_operator());
        //         }

        if constexpr (std::is_floating_point_v<Element>)
        {
            // a * b, b * a, a *= b
            class_
                .def(
                    "__mul__",
                    [](Scalar const& argument1, Scalar const& argument2)
                    { return multiply(argument1, argument2); },
                    pybind11::is_operator())
                .def(
                    "__mul__",
                    [](Scalar const& argument1, Element const argument2)
                    { return multiply(argument1, argument2); },
                    pybind11::is_operator())
                .def(
                    "__mul__",
                    [](Scalar const& argument1, Value const& argument2)
                    { return multiply(argument1, argument2); },
                    pybind11::is_operator())
                .def(
                    "__rmul__",
                    [](Scalar const& argument2, Element const argument1)
                    { return multiply(argument1, argument2); },
                    pybind11::is_operator())
                .def(
                    "__rmul__",
                    [](Scalar const& argument2, Value const& argument1)
                    { return multiply(argument1, argument2); },
                    pybind11::is_operator());

            // a / b, b / a, a /= b
            class_
                .def(
                    "__truediv__",
                    [](Scalar const& argument1, Scalar const& argument2)
                    { return divide(argument1, argument2); },
                    pybind11::is_operator())
                .def(
                    "__truediv__",
                    [](Scalar const& argument1, Element const argument2)
                    { return divide(argument1, argument2); },
                    pybind11::is_operator())
                .def(
                    "__truediv__",
                    [](Scalar const& argument1, Value const& argument2)
                    { return divide(argument1, argument2); },
                    pybind11::is_operator())
                .def(
                    "__rtruediv__",
                    [](Scalar const& argument2, Element const argument1)
                    { return divide(argument1, argument2); },
                    pybind11::is_operator())
                .def(
                    "__rtruediv__",
                    [](Scalar const& argument2, Value const& argument1)
                    { return divide(argument1, argument2); },
                    pybind11::is_operator());

            // a ** b, b ** a, a **= b
            class_
                .def(
                    "__pow__",
                    [](Scalar const& argument1, Scalar const& argument2)
                    { return pow(argument1, argument2); },
                    pybind11::is_operator())
                .def(
                    "__pow__",
                    [](Scalar const& argument1, Element const argument2)
                    { return pow(argument1, argument2); },
                    pybind11::is_operator())
                .def(
                    "__pow__",
                    [](Scalar const& argument1, Value const& argument2) { return pow(argument1, argument2); },
                    pybind11::is_operator())
                .def(
                    "__rpow__",
                    [](Scalar const& argument2, Element const argument1)
                    { return pow(argument1, argument2); },
                    pybind11::is_operator())
                .def(
                    "__rpow__",
                    [](Scalar const& argument2, Value const& argument1) { return pow(argument1, argument2); },
                    pybind11::is_operator());
        }

        module.def(
            "create_scalar",
            create_scalar_py,
            R"(
    Create new scalar, with a value

    :param numpy.dtype dtype: Description of the type of the scalar value
    :param value: Value to initialize scalar with
)",
            "dtype"_a,
            "value"_a);
    }

}  // namespace lue::framework
