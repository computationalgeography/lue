#include "lue/py/framework/core/scalar.hpp"
#include "lue/framework/configure.hpp"
#include "lue/concept.hpp"


using namespace pybind11::literals;


namespace lue::framework {
    namespace {

        template<typename Element>
        auto create_scalar(pybind11::object const& value) -> Scalar<Element>
        {
            return pybind11::cast<Element>(value);
        }


        auto create_scalar_py(pybind11::object const& dtype_args, pybind11::object const& value)
            -> pybind11::object
        {
            pybind11::dtype const dtype{pybind11::dtype::from_args(dtype_args)};

            // Switch on dtype and return a scalar of the right type

            auto const kind = dtype.kind();
            auto const size = dtype.itemsize();  // bytes
            pybind11::object result;

            switch (kind)
            {
                case 'i':
                {
                    // Signed integer
                    switch (size)
                    {
                        case 4:
                        {
                            result = pybind11::cast(create_scalar<std::int32_t>(value));
                            break;
                        }
                        case 8:
                        {
                            result = pybind11::cast(create_scalar<std::int64_t>(value));
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
                            result = pybind11::cast(create_scalar<std::uint8_t>(value));
                            break;
                        }
                        case 4:
                        {
                            result = pybind11::cast(create_scalar<std::uint32_t>(value));
                            break;
                        }
                        case 8:
                        {
                            result = pybind11::cast(create_scalar<std::uint64_t>(value));
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
                            result = pybind11::cast(create_scalar<float>(value));
                            break;
                        }
                        case 8:
                        {
                            result = pybind11::cast(create_scalar<double>(value));
                            break;
                        }
                    }

                    break;
                }
            }

            if (!result)
            {
                throw std::runtime_error(fmt::format("Unsupported dtype (kind={}, itemsize={})", kind, size));
            }

            return result;
        }

    }  // Anonymous namespace

    namespace {

        template<Arithmetic Element>
        void bind(pybind11::module& module)
        {
            bind_scalar<Element>(module);
        }


        template<TupleLike Elements, std::size_t idx>
        void bind(pybind11::module& module) requires(idx == 0)
        {
            bind<std::tuple_element_t<idx, Elements>>(module);
        }


        template<TupleLike Elements, std::size_t idx>
        void bind(pybind11::module& module) requires(idx > 0)
        {
            bind<std::tuple_element_t<idx, Elements>>(module);
            bind<Elements, idx - 1>(module);
        }


        template<TupleLike Elements>
        void bind(pybind11::module& module)
        {
            bind<Elements, std::tuple_size_v<Elements> - 1>(module);
        }

    }  // Anonymous namespace


    void bind_scalar(pybind11::module& module)
    {
        module.def(
            "create_scalar",
            create_scalar_py,
            R"(
    Create new scalar, with a value

    :param numpy.dtype dtype: Description of the type of the scalar value
    :param value: Value to initialize scalar with
)",
            "dtype"_a,
            "value"_a);

        bind<ArithmeticElements>(module);
    }

}  // namespace lue::framework
