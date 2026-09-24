#include "lue/framework/api/cxx/scalar.hpp"
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


        // TODO: Define and use a Scalar concept here
        template<typename Scalar>
        auto dtype() -> pybind11::dtype
        {
            return pybind11::dtype::of<ElementT<Scalar>>();
        }


        auto dtype(Scalar const& scalar) -> pybind11::dtype
        {
            return std::visit(
                overload{
                    [](auto const& scalar) -> pybind11::dtype { return api::dtype<decltype(scalar)>(); }},
                scalar.variant());
        }

    }  // Anonymous namespace


    void bind_scalar(pybind11::module& module)
    {
        pybind11::class_<Scalar>(module, "Scalar")

            // // bool(a), not a, if a, while a, ...
            // .def(
            //     "__bool__",
            //     []([[maybe_unused]] Scalar const& scalar) -> void
            //     {
            //         // ValueError
            //         throw std::invalid_argument("The truth value of a scalar is ambiguous");
            //     })

            // a + b, a += b
            // .def(pybind11::self + pybind11::self)
            .def("__add__", add, pybind11::is_operator())
            .def("__radd__", add, pybind11::is_operator())

            // TODO
            // https://pybind11.readthedocs.io/en/stable/advanced/classes.html#operator-overloading

            .def_property_readonly("dtype", [](Scalar const& self) -> pybind11::dtype { return dtype(self); })

            ;
    }

}  // namespace lue::api
