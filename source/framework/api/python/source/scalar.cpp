#include "lue/framework/api/cxx/scalar.hpp"
#include "lue/framework/api/cxx/create_scalar.hpp"
#include "lue/framework.hpp"
#include <fmt/format.h>
#include <pybind11/numpy.h>


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

    }  // Anonymous namespace

    void bind_scalar(pybind11::module& module)
    {
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
