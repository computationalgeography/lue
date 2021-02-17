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
#include "lue/framework/core/component/partitioned_array.hpp"
#include <pybind11/stl.h>


namespace lue::framework {
    namespace {

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

            // a < b
            .def("__lt__", [](Array const& argument1, Array const& argument2)
                { return less_than(argument1, argument2); },
                pybind11::is_operator())
            .def("__lt__", [](Array const& argument1, Element const argument2)
                { return less_than(argument1, argument2); },
                pybind11::is_operator())
            .def("__lt__", [](Array const& argument1, ElementF const& argument2)
                { return less_than(argument1, argument2); },
                pybind11::is_operator())

            // a <= b
            .def("__le__", [](Array const& argument1, Array const& argument2)
                { return less_than_equal_to(argument1, argument2); },
                pybind11::is_operator())
            .def("__le__", [](Array const& argument1, Element const argument2)
                { return less_than_equal_to(argument1, argument2); },
                pybind11::is_operator())
            .def("__le__", [](Array const& argument1, ElementF const& argument2)
                { return less_than_equal_to(argument1, argument2); },
                pybind11::is_operator())

            // a == b
            .def("__eq__", [](Array const& argument1, Array const& argument2)
                { return equal_to(argument1, argument2); },
                pybind11::is_operator())
            .def("__eq__", [](Array const& argument1, Element const argument2)
                { return equal_to(argument1, argument2); },
                pybind11::is_operator())
            .def("__eq__", [](Array const& argument1, ElementF const& argument2)
                { return equal_to(argument1, argument2); },
                pybind11::is_operator())

            // a != b
            .def("__ne__", [](Array const& argument1, Array const& argument2)
                { return not_equal_to(argument1, argument2); },
                pybind11::is_operator())
            .def("__ne__", [](Array const& argument1, Element const argument2)
                { return not_equal_to(argument1, argument2); },
                pybind11::is_operator())
            .def("__ne__", [](Array const& argument1, ElementF const& argument2)
                { return not_equal_to(argument1, argument2); },
                pybind11::is_operator())

            // a > b
            .def("__gt__", [](Array const& argument1, Array const& argument2)
                { return greater_than(argument1, argument2); },
                pybind11::is_operator())
            .def("__gt__", [](Array const& argument1, Element const argument2)
                { return greater_than(argument1, argument2); },
                pybind11::is_operator())
            .def("__gt__", [](Array const& argument1, ElementF const& argument2)
                { return greater_than(argument1, argument2); },
                pybind11::is_operator())

            // a >= b
            .def("__ge__", [](Array const& argument1, Array const& argument2)
                { return greater_than_equal_to(argument1, argument2); },
                pybind11::is_operator())
            .def("__ge__", [](Array const& argument1, Element const argument2)
                { return greater_than_equal_to(argument1, argument2); },
                pybind11::is_operator())
            .def("__ge__", [](Array const& argument1, ElementF const& argument2)
                { return greater_than_equal_to(argument1, argument2); },
                pybind11::is_operator())

            ;


            // a + b, b + a, a += b
            class_
                .def("__add__", [](Array const& argument1, Array const& argument2)
                    { return add(argument1, argument2); },
                    pybind11::is_operator())
                .def("__add__", [](Array const& argument1, Element const argument2)
                    { return add(argument1, argument2); },
                    pybind11::is_operator())
                .def("__add__", [](Array const& argument1, ElementF const& argument2)
                    { return add(argument1, argument2); },
                    pybind11::is_operator())
                .def("__radd__", [](Array const& argument2, Element const argument1)
                    { return add(argument1, argument2); },
                    pybind11::is_operator())
                .def("__radd__", [](Array const& argument2, ElementF const& argument1)
                    { return add(argument1, argument2); },
                    pybind11::is_operator())
                ;

            // a - b, b - a, a -= b
            class_
                .def("__sub__", [](Array const& argument1, Array const& argument2)
                    { return subtract(argument1, argument2); },
                    pybind11::is_operator())
                .def("__sub__", [](Array const& argument1, Element const argument2)
                    { return subtract(argument1, argument2); },
                    pybind11::is_operator())
                .def("__sub__", [](Array const& argument1, ElementF const& argument2)
                    { return subtract(argument1, argument2); },
                    pybind11::is_operator())
                .def("__rsub__", [](Array const& argument2, Element const argument1)
                    { return subtract(argument1, argument2); },
                    pybind11::is_operator())
                .def("__rsub__", [](Array const& argument2, ElementF const& argument1)
                    { return subtract(argument1, argument2); },
                    pybind11::is_operator())
                ;

            if constexpr (std::is_floating_point_v<Element>)
            {
                // a * b, b * a, a *= b
                class_
                    .def("__mul__", [](Array const& argument1, Array const& argument2)
                        { return multiply(argument1, argument2); },
                        pybind11::is_operator())
                    .def("__mul__", [](Array const& argument1, Element const argument2)
                        { return multiply(argument1, argument2); },
                        pybind11::is_operator())
                    .def("__mul__", [](Array const& argument1, ElementF const& argument2)
                        { return multiply(argument1, argument2); },
                        pybind11::is_operator())
                    .def("__rmul__", [](Array const& argument2, Element const argument1)
                        { return multiply(argument1, argument2); },
                        pybind11::is_operator())
                    .def("__rmul__", [](Array const& argument2, ElementF const& argument1)
                        { return multiply(argument1, argument2); },
                        pybind11::is_operator())
                    ;

                // a / b, b / a, a /= b
                class_
                    .def("__truediv__", [](Array const& argument1, Array const& argument2)
                        { return divide(argument1, argument2); },
                        pybind11::is_operator())
                    .def("__truediv__", [](Array const& argument1, Element const argument2)
                        { return divide(argument1, argument2); },
                        pybind11::is_operator())
                    .def("__truediv__", [](Array const& argument1, ElementF const& argument2)
                        { return divide(argument1, argument2); },
                        pybind11::is_operator())
                    .def("__rtruediv__", [](Array const& argument2, Element const argument1)
                        { return divide(argument1, argument2); },
                        pybind11::is_operator())
                    .def("__rtruediv__", [](Array const& argument2, ElementF const& argument1)
                        { return divide(argument1, argument2); },
                        pybind11::is_operator())
                    ;

                // a ** b, b ** a, a **= b
                class_
                    .def("__pow__", [](Array const& argument1, Array const& argument2)
                        { return pow(argument1, argument2); },
                        pybind11::is_operator())
                    .def("__pow__", [](Array const& argument1, Element const argument2)
                        { return pow(argument1, argument2); },
                        pybind11::is_operator())
                    .def("__pow__", [](Array const& argument1, ElementF const& argument2)
                        { return pow(argument1, argument2); },
                        pybind11::is_operator())
                    .def("__rpow__", [](Array const& argument2, Element const argument1)
                        { return pow(argument1, argument2); },
                        pybind11::is_operator())
                    .def("__rpow__", [](Array const& argument2, ElementF const& argument1)
                        { return pow(argument1, argument2); },
                        pybind11::is_operator())
                    ;
            }
        }

    }  // Anonymous namespace


    void bind_partitioned_array(
        pybind11::module& module)
    {
        // TODO: https://github.com/pybind/pybind11/issues/199#issuecomment-323995589
        bind_partitioned_array<std::uint8_t, 2>(module);
        bind_partitioned_array<std::uint32_t, 2>(module);
        bind_partitioned_array<std::uint64_t, 2>(module);
        bind_partitioned_array<std::int32_t, 2>(module);
        bind_partitioned_array<std::int64_t, 2>(module);
        bind_partitioned_array<float, 2>(module);
        bind_partitioned_array<double, 2>(module);
    }

}  // namespace lue::framework
