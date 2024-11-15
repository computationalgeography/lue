#include "lue/framework/api/cxx/scalar.hpp"
#include "lue/framework/api/cxx/create_scalar.hpp"
#include "lue/framework.hpp"
#include <fmt/format.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>


using namespace pybind11::literals;


namespace lue::api {

    namespace {

        template<typename Element>
        auto create_scalar(double const value) -> std::optional<Field>
        {
            std::optional<Field> result{};

            if constexpr (lue::arithmetic_element_supported<Element>)
            {
                result = lue::api::create_scalar(static_cast<Element>(value));
            }

            return result;
        }


        auto create_scalar(double const value, pybind11::dtype const& dtype) -> Field
        {
            // TODO Out of range values must result in no-data values. This logic must be in the API layer or
            // higher. All bindings need it.
            //
            // auto output_value = value_policies::cast<std::int32_t>(input_value);

            auto const kind = dtype.kind();
            auto const size = dtype.itemsize();  // bytes
            std::optional<Field> field{};

            switch (kind)
            {
                case 'i':
                {
                    // Signed integer
                    switch (size)
                    {
                        case 1:
                        {
                            field = create_scalar<std::int8_t>(value);
                            break;
                        }
                        case 4:
                        {
                            field = create_scalar<std::int32_t>(value);
                            break;
                        }
                        case 8:
                        {
                            field = create_scalar<std::int64_t>(value);
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
                            field = create_scalar<std::uint8_t>(value);
                            break;
                        }
                        case 4:
                        {
                            field = create_scalar<std::uint32_t>(value);
                            break;
                        }
                        case 8:
                        {
                            field = create_scalar<std::uint64_t>(value);
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
                            field = create_scalar<float>(value);
                            break;
                        }
                        case 8:
                        {
                            field = create_scalar<double>(value);
                            break;
                        }
                    }

                    break;
                }
            }

            if (!field)
            {
                throw std::runtime_error(fmt::format("Unsupported dtype (kind={}, itemsize={})", kind, size));
            }

            return std::move(*field);
        }


        template<typename... Elements>
        auto dtype_of([[maybe_unused]] std::tuple<Elements...>&& elements)
            -> std::array<pybind11::dtype, sizeof...(Elements)>
        {
            return std::array<pybind11::dtype, sizeof...(Elements)>{(pybind11::dtype::of<Elements>(), ...)};
        }


        template<typename... Elements>
        auto dtype_of2([[maybe_unused]] std::tuple<Elements...>&& elements) -> std::vector<pybind11::dtype>
        {
            return std::vector<pybind11::dtype>{(pybind11::dtype::of<Elements>(), ...)};
        }

    }  // Anonymous namespace


    // TODO Find a way to do this without creating an ArithmeticElements instance. We only have to iterate
    //      over the tuple's types.
    // TODO Find a way to use an array instead of a vector. Not crucial, but that's what it is. Results in
    //      runtime error:
    //          ImportError: generic_type: cannot initialize type "VectorHSizeT": an object with that name is
    //          already defined
    // static std::array<pybind11::dtype, nr_arithmetic_elements> const arithmetic_element_types =
    // dtype_of(ArithmeticElements{});

    static std::vector<pybind11::dtype> const arithmetic_element_types = dtype_of2(ArithmeticElements{});

    void bind_scalar(pybind11::module& module)
    {
        module.attr("arithmetic_element_types") = arithmetic_element_types;

        module.def(
            "create_scalar",
            [](double const value, pybind11::object const& dtype_args) -> Field
            {
                pybind11::dtype const dtype{pybind11::dtype::from_args(dtype_args)};

                return create_scalar(value, dtype);
            },
            "value"_a,
            "dtype"_a,
            pybind11::return_value_policy::move);
    }

}  // namespace lue::api
