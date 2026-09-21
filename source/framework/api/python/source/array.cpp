#include "lue/framework/api/cxx/array.hpp"
#include "lue/overload.hpp"
#include "lue/framework/api/cxx/local/add.hpp"
#include "lue/framework/api/cxx/local/subtract.hpp"
#include <pybind11/numpy.h>
#include <pybind11/stl.h>


using namespace pybind11::literals;


namespace lue::api {

    namespace {

        // TODO: Define and use a Array concept here
        template<typename Array>
        auto dtype() -> pybind11::dtype
        {
            return pybind11::dtype::of<ElementT<Array>>();
        }


        auto dtype(Array const& array) -> pybind11::dtype
        {
            return std::visit(
                overload{[](auto const& array) -> pybind11::dtype { return api::dtype<decltype(array)>(); }},
                array.variant());
        }


        template<Arithmetic Element>
        auto shape(PartitionedArray<Element, 2> const& array) -> std::tuple<Count, Count>
        {
            return {array.shape()[0], array.shape()[1]};
        }


        auto shape(Array const& array) -> std::tuple<Count, Count>
        {
            return std::visit(
                overload{[](auto const& array) -> std::tuple<Count, Count> { return api::shape(array); }},
                array.variant());
        }

    }  // Anonymous namespace


    void bind_array(pybind11::module& module)
    {
        pybind11::class_<Array>(module, "Array")

            // bool(a), not a, if a, while a, ...
            .def(
                "__bool__",
                []([[maybe_unused]] Array const& Array) -> void
                {
                    // ValueError
                    throw std::invalid_argument("The truth value of an Array is ambiguous");
                })

            // a + b, a += b
            // .def(pybind11::self + pybind11::self)
            .def("__add__", add, pybind11::is_operator())
            .def("__radd__", add, pybind11::is_operator())


            // TODO
            // https://pybind11.readthedocs.io/en/stable/advanced/classes.html#operator-overloading

            .def_property_readonly(
                "dtype",
                [](Array const& self) -> pybind11::dtype { return dtype(self); })

            .def_property_readonly(
                "shape",
                [](Array const& self) -> std::tuple<Count, Count> { return shape(self); })

            ;
    }

}  // namespace lue::api
