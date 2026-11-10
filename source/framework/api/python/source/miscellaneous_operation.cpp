#include "lue/framework/api/cxx/miscellaneous_operation.hpp"
#include "switch_on_dtype.hpp"
#include "lue/framework.hpp"
#include <pybind11/numpy.h>
#include <pybind11/stl.h>


using namespace pybind11::literals;


namespace lue::api {

    namespace {

        struct Caster
        {
                template<Arithmetic Element>
                static auto operator()(Field const& field) -> Field
                {
                    return lue::api::cast(field, TypeTraits<Element>::element_type);
                }
        };


        auto cast(Field const& field, pybind11::dtype const& dtype) -> Field
        {
            return switch_on_dtype<Field, Caster>(dtype, field);
        }


        struct LiteralCreator
        {
                template<Arithmetic Element>
                static auto operator()(double const value) -> std::optional<Literal>
                {
                    std::optional<Literal> result{};

                    if constexpr (lue::arithmetic_element_supported<Element>)
                    {
                        // TODO: Use cast which handles out of range correctly
                        result = static_cast<Element>(value);
                    }

                    return result;
                }
        };


        auto create_literal(double const value, pybind11::dtype const& dtype) -> Literal
        {
            return switch_on_dtype<Literal, LiteralCreator>(dtype, value);
        }


        struct ScalarCreator
        {
                template<Arithmetic Element>
                static auto operator()(double const value) -> std::optional<Scalar>
                {
                    std::optional<Scalar> result{};

                    if constexpr (lue::arithmetic_element_supported<Element>)
                    {
                        // TODO: Use cast which handles out of range correctly
                        result = lue::api::create_scalar(static_cast<Element>(value));
                    }

                    return result;
                }
        };


        auto create_scalar(double const value, pybind11::dtype const& dtype) -> Scalar
        {
            return switch_on_dtype<Scalar, ScalarCreator>(dtype, value);
        }

    }  // Anonymous namespace


    void bind_miscellaneous_operations(pybind11::module& module)
    {
        module.def(
            "array_like",
            [](Array const& array, Scalar const& fill_value) -> Array
            { return array_like(array, fill_value); },
            "array"_a,
            "fill_value"_a);

        module.def(
            "cast",
            [](Field const& field, pybind11::object const& dtype_args) -> Field
            {
                pybind11::dtype const dtype{pybind11::dtype::from_args(dtype_args)};

                return cast(field, dtype);
            },
            "field"_a,
            "dtype"_a);

        module.def("cell_index", cell_index, "condition"_a, "dimension_index"_a);

        module.def(
            "create_array",
            [](Shape<Count, 2> const& array_shape,
               Scalar const& fill_value,
               std::optional<Shape<Count, 2>> const& partition_shape) -> Array
            {
                return partition_shape ? create_array(array_shape, *partition_shape, fill_value)
                                       : create_array(array_shape, fill_value);
            },
            "array_shape"_a,
            "fill_value"_a,
            pybind11::kw_only(),
            "partition_shape"_a = std::optional<Shape<Count, 2>>{});

        module.def(
            "create_literal",
            [](double const value, pybind11::object const& dtype_args) -> Literal
            {
                pybind11::dtype const dtype{pybind11::dtype::from_args(dtype_args)};

                return create_literal(value, dtype);
            },
            "value"_a,
            "dtype"_a);

        module.def(
            "create_scalar",
            [](double const value, pybind11::object const& dtype_args) -> Scalar
            {
                pybind11::dtype const dtype{pybind11::dtype::from_args(dtype_args)};

                return create_scalar(value, dtype);
            },
            "value"_a,
            "dtype"_a);

        module.def("reclassify", reclassify, "field"_a, "lookup_table"_a);
    }

}  // namespace lue::api
