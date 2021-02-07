#include "lue/py/framework/algorithm/add.hpp"
#include "lue/py/framework/algorithm/divide.hpp"
#include "lue/py/framework/algorithm/multiply.hpp"
#include "lue/py/framework/algorithm/subtract.hpp"
#include "lue/py/framework/stream.hpp"
#include "lue/framework/core/component/partitioned_array.hpp"
#include <pybind11/pybind11.h>


// TODO Make array behave like numeric type:
// https://docs.python.org/3/reference/datamodel.html?highlight=rmul#emulating-numeric-types
// -, /
// ...


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
                .def("__radd__", [](Element const argument1, Array const& argument2)
                    { return add(argument1, argument2); },
                    pybind11::is_operator())
                .def("__radd__", [](Array const& argument2, ElementF const& argument1)
                    { return add(argument1, argument2); },
                    pybind11::is_operator())
                ;

            // a - b, b - a, a -= b
            class_
                .def("__sub__", [](Array const& argument1, Array const& argument2)
                    { return sub(argument1, argument2); },
                    pybind11::is_operator())
                .def("__sub__", [](Array const& argument1, Element const argument2)
                    { return sub(argument1, argument2); },
                    pybind11::is_operator())
                .def("__sub__", [](Array const& argument1, ElementF const& argument2)
                    { return sub(argument1, argument2); },
                    pybind11::is_operator())
                .def("__rsub__", [](Element const argument1, Array const& argument2)
                    { return sub(argument1, argument2); },
                    pybind11::is_operator())
                .def("__rsub__", [](Array const& argument2, ElementF const& argument1)
                    { return sub(argument1, argument2); },
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
