#include "lue/framework/api/cxx/create_scalar.hpp"
#include <fmt/format.h>
#include <pybind11/numpy.h>
#include <pybind11/operators.h>


using namespace pybind11::literals;


namespace lue::api {

    namespace {

        auto create_scalar(double const value, pybind11::dtype const& dtype) -> Scalar
        {
            // TODO Out of range values must result in no-data values. This logic must be in the API layer or
            // higher. All bindings need it.
            //
            // auto output_value = value_policies::cast<std::int32_t>(input_value);

            auto const kind = dtype.kind();
            auto const size = dtype.itemsize();  // bytes
            std::optional<Scalar> scalar{};

            switch (kind)
            {
                case 'i':
                {
                    // Signed integer
                    switch (size)
                    {
                        case 4:
                        {
                            scalar = lue::api::create_scalar(static_cast<std::int32_t>(value));
                            break;
                        }
                        case 8:
                        {
                            scalar = lue::api::create_scalar(static_cast<std::int64_t>(value));
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
                            scalar = lue::api::create_scalar(static_cast<std::uint8_t>(value));
                            break;
                        }
                        case 4:
                        {
                            scalar = lue::api::create_scalar(static_cast<std::uint32_t>(value));
                            break;
                        }
                        case 8:
                        {
                            scalar = lue::api::create_scalar(static_cast<std::uint64_t>(value));
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
                            scalar = lue::api::create_scalar(static_cast<float>(value));
                            break;
                        }
                        case 8:
                        {
                            scalar = lue::api::create_scalar(value);
                            break;
                        }
                    }

                    break;
                }
            }

            if (!scalar)
            {
                throw std::runtime_error(fmt::format("Unsupported dtype (kind={}, itemsize={})", kind,
                size));
            }

            return *scalar;
        }

    }  // Anonymous namespace


    void bind_scalar(pybind11::module& module)
    {

        pybind11::class_<Scalar>(module, "Scalar")

            .def(pybind11::init(
                [](
                   double const value,
                   pybind11::object const& dtype_args)
                {
                    pybind11::dtype const dtype{pybind11::dtype::from_args(dtype_args)};

                    return create_scalar(value, dtype);
                }))

            // bool(a), not a, if a, while a, ...
            .def(
                "__bool__",
                []([[maybe_unused]] Scalar const& scalar)
                {
                    // ValueError
                    throw std::invalid_argument("The truth value of a scalar is ambiguous");
                })

            ;
    }

}  // namespace lue::api
