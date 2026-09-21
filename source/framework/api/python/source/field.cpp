#include "lue/overload.hpp"
#include "lue/framework/api/cxx/local/add.hpp"
#include "lue/framework/api/cxx/local/subtract.hpp"
#include "lue/framework/api/cxx/type_info.hpp"
#include <pybind11/numpy.h>
#include <pybind11/operators.h>


namespace lue::api {

    namespace {

        // TODO: Define and use a Field concept here
        template<typename Field>
        auto dtype() -> pybind11::dtype
        {
            return pybind11::dtype::of<ElementT<Field>>();
        }


        auto dtype(Field const& field) -> pybind11::dtype
        {
            return std::visit(
                overload{[](auto const& field) -> pybind11::dtype { return api::dtype<decltype(field)>(); }},
                field.variant());
        }

    }  // Anonymous namespace


    void bind_field(pybind11::module& module)
    {
        pybind11::class_<Field>(module, "Field")

            // bool(a), not a, if a, while a, ...
            .def(
                "__bool__",
                []([[maybe_unused]] Field const& field) -> void
                {
                    // ValueError
                    throw std::invalid_argument("The truth value of a field is ambiguous");
                })

            // a + b, a += b
            // .def(pybind11::self + pybind11::self)
            .def("__add__", add, pybind11::is_operator())
            .def("__radd__", add, pybind11::is_operator())

            // a - b, a -= b
            // .def(pybind11::self - pybind11::self)
            .def("__sub__", subtract, pybind11::is_operator())
            .def("__rsub__", subtract, pybind11::is_operator())

            // TODO
            // https://pybind11.readthedocs.io/en/stable/advanced/classes.html#operator-overloading

            .def_property_readonly(
                "data_model",
                [](Field const& self) -> DataModel { return data_model(self); })

            .def_property_readonly(
                "dtype",
                [](Field const& self) -> pybind11::dtype { return dtype(self); })

            ;
    }

}  // namespace lue::api
