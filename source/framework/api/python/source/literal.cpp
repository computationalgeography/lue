#include "lue/framework/api/cxx/literal.hpp"
#include "lue/overload.hpp"
#include "lue/framework/api/cxx/local/add.hpp"
#include "lue/framework/api/cxx/local/subtract.hpp"
#include <pybind11/numpy.h>


namespace lue::api {

    namespace {

        template<Arithmetic Element>
        auto dtype() -> pybind11::dtype
        {
            return pybind11::dtype::of<Element>();
        }


        // TODO: Define and use a Literal concept here
        template<typename Literal>
        auto dtype() -> pybind11::dtype
        {
            return pybind11::dtype::of<ElementT<Literal>>();
        }


        auto dtype(Literal const& literal) -> pybind11::dtype
        {
            return std::visit(
                overload{
                    [](auto const literal) -> pybind11::dtype { return api::dtype<decltype(literal)>(); }},
                literal.variant());
        }

    }  // Anonymous namespace


    void bind_literal(pybind11::module& module)
    {
        pybind11::class_<Literal>(module, "Literal")

            // // bool(a), not a, if a, while a, ...
            // .def(
            //     "__bool__",
            //     []([[maybe_unused]] Literal const& literal) -> void
            //     {
            //         // ValueError
            //         throw std::invalid_argument("The truth value of a literal is ambiguous");
            //     })

            // a + b, a += b
            // .def(pybind11::self + pybind11::self)
            .def("__add__", add, pybind11::is_operator())
            .def("__radd__", add, pybind11::is_operator())

            // TODO
            // https://pybind11.readthedocs.io/en/stable/advanced/classes.html#operator-overloading

            .def_property_readonly(
                "dtype", [](Literal const& self) -> pybind11::dtype { return dtype(self); })

            ;
    }

}  // namespace lue::api
