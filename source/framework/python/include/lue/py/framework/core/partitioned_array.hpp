#include "lue/py/framework/algorithm/abs.hpp"
#include "lue/py/framework/algorithm/add.hpp"
#include "lue/py/framework/algorithm/divide.hpp"
#include "lue/py/framework/algorithm/equal_to.hpp"
#include "lue/py/framework/algorithm/greater_than.hpp"
#include "lue/py/framework/algorithm/greater_than_equal_to.hpp"
#include "lue/py/framework/algorithm/less_than.hpp"
#include "lue/py/framework/algorithm/less_than_equal_to.hpp"
#include "lue/py/framework/algorithm/multiply.hpp"
#include "lue/py/framework/algorithm/not_equal_to.hpp"
#include "lue/py/framework/algorithm/pow.hpp"
#include "lue/py/framework/algorithm/subtract.hpp"
#include "lue/py/framework/stream.hpp"
#include "lue/py/configure.hpp"
#include <pybind11/stl.h>


namespace lue::framework {

    template<
        typename Element,
        Rank rank>
    static std::string formal_string_representation(
        PartitionedArray<Element, rank> const& array)
    {
        return fmt::format(
            "PartitionedArray<{}, {}>{{shape={}, shape_in_partitions={}}}",
            as_string<Element>(), rank,
            as_string(array.shape()),
            as_string(array.partitions().shape()));
    }


    template<
        typename Element,
        Rank rank>
    static std::string informal_string_representation(
        PartitionedArray<Element, rank> const& array)
    {
        return formal_string_representation(array);
    }


    template<
        typename Element,
        Rank rank>
    void bind_partitioned_array(
        pybind11::module& module)
    {
        namespace lfr = lue::framework;

        using Array = PartitionedArray<Element, rank>;
        using ElementF = hpx::shared_future<Element>;

        auto class_ = pybind11::class_<Array>(
            module,
            fmt::format("PartitionedArray_{}_{}", as_string<Element>(), rank).c_str(),
            fmt::format(R"(
    Partitioned array type for arrays of rank {}, containing array
    elements of type {}
)"
                , rank, as_string<Element>()).c_str())

        .def_property_readonly(
            "dtype",
            []([[maybe_unused]] Array const& self)
            {
                return dtype<Element>();
            },
            "dtype docstring..."
        )

        .def_property_readonly(
            "shape",
            [](Array const& self)
            {
                static_assert(rank == 2);

                auto shape{self.shape()};

                return std::make_tuple(shape[0], shape[1]);
            },
            "shape docstring..."
        )

        .def_property_readonly(
            "shape_in_partitions",
            [](Array const& self)
            {
                static_assert(rank == 2);

                auto shape{self.partitions().shape()};

                return std::make_tuple(shape[0], shape[1]);
            },
            "shape_in_partitions docstring..."
        )

        .def(
            "__repr__",
            [](Array const& array) {
                return formal_string_representation(array);
            }
        )

        .def(
            "__str__",
            [](Array const& array) {
                return informal_string_representation(array);
            }
        )

        // bool(a), not a, if a, while a, ...
        .def("__bool__", []([[maybe_unused]] Array const& argument)
            {
                // ValueError
                throw std::invalid_argument("The truth value of an array is ambiguous");
            })

        // a < b
        .def("__lt__", [](Array const& argument1, Array const& argument2)
            { return lfr::less_than<ElementT<Array>, std::uint8_t, rank>(argument1, argument2); },
            pybind11::is_operator())
        .def("__lt__", [](Array const& argument1, Element const argument2)
            { return lfr::less_than<ElementT<Array>, std::uint8_t, rank>(argument1, argument2); },
            pybind11::is_operator())
        .def("__lt__", [](Array const& argument1, ElementF const& argument2)
            { return lfr::less_than<ElementT<Array>, std::uint8_t, rank>(argument1, argument2); },
            pybind11::is_operator())

        // a <= b
        .def("__le__", [](Array const& argument1, Array const& argument2)
            { return lfr::less_than_equal_to<ElementT<Array>, std::uint8_t, rank>(argument1, argument2); },
            pybind11::is_operator())
        .def("__le__", [](Array const& argument1, Element const argument2)
            { return lfr::less_than_equal_to<ElementT<Array>, std::uint8_t, rank>(argument1, argument2); },
            pybind11::is_operator())
        .def("__le__", [](Array const& argument1, ElementF const& argument2)
            { return lfr::less_than_equal_to<ElementT<Array>, std::uint8_t, rank>(argument1, argument2); },
            pybind11::is_operator())

        // a == b
        .def("__eq__", [](Array const& argument1, Array const& argument2)
            { return lfr::equal_to<ElementT<Array>, std::uint8_t, rank>(argument1, argument2); },
            pybind11::is_operator())
        .def("__eq__", [](Array const& argument1, Element const argument2)
            { return lfr::equal_to<ElementT<Array>, std::uint8_t, rank>(argument1, argument2); },
            pybind11::is_operator())
        .def("__eq__", [](Array const& argument1, ElementF const& argument2)
            { return lfr::equal_to<ElementT<Array>, std::uint8_t, rank>(argument1, argument2); },
            pybind11::is_operator())

        // a != b
        .def("__ne__", [](Array const& argument1, Array const& argument2)
            { return lfr::not_equal_to<ElementT<Array>, std::uint8_t, rank>(argument1, argument2); },
            pybind11::is_operator())
        .def("__ne__", [](Array const& argument1, Element const argument2)
            { return lfr::not_equal_to<ElementT<Array>, std::uint8_t, rank>(argument1, argument2); },
            pybind11::is_operator())
        .def("__ne__", [](Array const& argument1, ElementF const& argument2)
            { return lfr::not_equal_to<ElementT<Array>, std::uint8_t, rank>(argument1, argument2); },
            pybind11::is_operator())

        // a > b
        .def("__gt__", [](Array const& argument1, Array const& argument2)
            { return lfr::greater_than<ElementT<Array>, std::uint8_t, rank>(argument1, argument2); },
            pybind11::is_operator())
        .def("__gt__", [](Array const& argument1, Element const argument2)
            { return lfr::greater_than<ElementT<Array>, std::uint8_t, rank>(argument1, argument2); },
            pybind11::is_operator())
        .def("__gt__", [](Array const& argument1, ElementF const& argument2)
            { return lfr::greater_than<ElementT<Array>, std::uint8_t, rank>(argument1, argument2); },
            pybind11::is_operator())

        // a >= b
        .def("__ge__", [](Array const& argument1, Array const& argument2)
            { return lfr::greater_than_equal_to<ElementT<Array>, std::uint8_t, rank>(argument1, argument2); },
            pybind11::is_operator())
        .def("__ge__", [](Array const& argument1, Element const argument2)
            { return lfr::greater_than_equal_to<ElementT<Array>, std::uint8_t, rank>(argument1, argument2); },
            pybind11::is_operator())
        .def("__ge__", [](Array const& argument1, ElementF const& argument2)
            { return lfr::greater_than_equal_to<ElementT<Array>, std::uint8_t, rank>(argument1, argument2); },
            pybind11::is_operator())

        ;


        // a + b, b + a, a += b
        class_
            .def("__add__", [](Array const& argument1, Array const& argument2)
                { return lfr::add<ElementT<Array>, rank>(argument1, argument2); },
                pybind11::is_operator())
            .def("__add__", [](Array const& argument1, Element const argument2)
                { return lfr::add<ElementT<Array>, rank>(argument1, argument2); },
                pybind11::is_operator())
            .def("__add__", [](Array const& argument1, ElementF const& argument2)
                { return lfr::add<ElementT<Array>, rank>(argument1, argument2); },
                pybind11::is_operator())
            .def("__radd__", [](Array const& argument2, Element const argument1)
                { return lfr::add<ElementT<Array>, rank>(argument1, argument2); },
                pybind11::is_operator())
            .def("__radd__", [](Array const& argument2, ElementF const& argument1)
                { return lfr::add<ElementT<Array>, rank>(argument1, argument2); },
                pybind11::is_operator())
            ;

        // a - b, b - a, a -= b
        class_
            .def("__sub__", [](Array const& argument1, Array const& argument2)
                { return lfr::subtract<ElementT<Array>, rank>(argument1, argument2); },
                pybind11::is_operator())
            .def("__sub__", [](Array const& argument1, Element const argument2)
                { return lfr::subtract<ElementT<Array>, rank>(argument1, argument2); },
                pybind11::is_operator())
            .def("__sub__", [](Array const& argument1, ElementF const& argument2)
                { return lfr::subtract<ElementT<Array>, rank>(argument1, argument2); },
                pybind11::is_operator())
            .def("__rsub__", [](Array const& argument2, Element const argument1)
                { return lfr::subtract<ElementT<Array>, rank>(argument1, argument2); },
                pybind11::is_operator())
            .def("__rsub__", [](Array const& argument2, ElementF const& argument1)
                { return lfr::subtract<ElementT<Array>, rank>(argument1, argument2); },
                pybind11::is_operator())
            ;

        if constexpr (std::is_signed_v<Element> || std::is_floating_point_v<Element>)
        {
            // abs(a)
            class_
                .def("__abs__", [](Array const& argument)
                    { return lfr::abs<ElementT<Array>, rank>(argument); },
                    pybind11::is_operator())
                ;
        }

        if constexpr (std::is_floating_point_v<Element>)
        {
            // a * b, b * a, a *= b
            class_
                .def("__mul__", [](Array const& argument1, Array const& argument2)
                    { return lfr::multiply<ElementT<Array>, rank>(argument1, argument2); },
                    pybind11::is_operator())
                .def("__mul__", [](Array const& argument1, Element const argument2)
                    { return lfr::multiply<ElementT<Array>, rank>(argument1, argument2); },
                    pybind11::is_operator())
                .def("__mul__", [](Array const& argument1, ElementF const& argument2)
                    { return lfr::multiply<ElementT<Array>, rank>(argument1, argument2); },
                    pybind11::is_operator())
                .def("__rmul__", [](Array const& argument2, Element const argument1)
                    { return lfr::multiply<ElementT<Array>, rank>(argument1, argument2); },
                    pybind11::is_operator())
                .def("__rmul__", [](Array const& argument2, ElementF const& argument1)
                    { return lfr::multiply<ElementT<Array>, rank>(argument1, argument2); },
                    pybind11::is_operator())
                ;

            // a / b, b / a, a /= b
            class_
                .def("__truediv__", [](Array const& argument1, Array const& argument2)
                    { return lfr::divide<ElementT<Array>, rank>(argument1, argument2); },
                    pybind11::is_operator())
                .def("__truediv__", [](Array const& argument1, Element const argument2)
                    { return lfr::divide<ElementT<Array>, rank>(argument1, argument2); },
                    pybind11::is_operator())
                .def("__truediv__", [](Array const& argument1, ElementF const& argument2)
                    { return lfr::divide<ElementT<Array>, rank>(argument1, argument2); },
                    pybind11::is_operator())
                .def("__rtruediv__", [](Array const& argument2, Element const argument1)
                    { return lfr::divide<ElementT<Array>, rank>(argument1, argument2); },
                    pybind11::is_operator())
                .def("__rtruediv__", [](Array const& argument2, ElementF const& argument1)
                    { return lfr::divide<ElementT<Array>, rank>(argument1, argument2); },
                    pybind11::is_operator())
                ;

            // a ** b, b ** a, a **= b
            class_
                .def("__pow__", [](Array const& argument1, Array const& argument2)
                    { return lfr::pow<ElementT<Array>, rank>(argument1, argument2); },
                    pybind11::is_operator())
                .def("__pow__", [](Array const& argument1, Element const argument2)
                    { return lfr::pow<ElementT<Array>, rank>(argument1, argument2); },
                    pybind11::is_operator())
                .def("__pow__", [](Array const& argument1, ElementF const& argument2)
                    { return lfr::pow<ElementT<Array>, rank>(argument1, argument2); },
                    pybind11::is_operator())
                .def("__rpow__", [](Array const& argument2, Element const argument1)
                    { return lfr::pow<ElementT<Array>, rank>(argument1, argument2); },
                    pybind11::is_operator())
                .def("__rpow__", [](Array const& argument2, ElementF const& argument1)
                    { return lfr::pow<ElementT<Array>, rank>(argument1, argument2); },
                    pybind11::is_operator())
                ;
        }
    }

}  // namespace lue::framework
