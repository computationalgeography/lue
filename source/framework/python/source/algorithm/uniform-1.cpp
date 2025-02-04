#include "lue/framework/algorithm/value_policies/uniform.hpp"
#include "lue/framework.hpp"
#include "lue/py/bind.hpp"
#include <pybind11/numpy.h>
// #include <pybind11/stl.h>


using namespace pybind11::literals;


// std::uint8_t is not supported:
// https://stackoverflow.com/questions/31460733/why-arent-stduniform-int-distributionuint8-t-and-stduniform-int-distri


namespace lue::framework {
    namespace {

        template<typename OutputElement, typename Element, Rank rank>
        auto uniform(
            PartitionedArray<Element, rank> const& array,
            pybind11::object const& min_value,
            pybind11::object const& max_value) -> pybind11::object
        {
            pybind11::object result{};

            if constexpr (arithmetic_element_supported<OutputElement>)
            {
                result = pybind11::cast(value_policies::uniform(
                    array,
                    pybind11::cast<OutputElement>(min_value),
                    pybind11::cast<OutputElement>(max_value)));
            }

            return result;
        }


        template<typename Element, Rank rank>
        auto uniform(
            PartitionedArray<Element, rank> const& array,
            pybind11::object const& dtype_args,
            pybind11::object const& min_value,
            pybind11::object const& max_value) -> pybind11::object
        {
            pybind11::dtype const dtype{pybind11::dtype::from_args(dtype_args)};

            // Switch on dtype and call a function that returns an array of the
            // right value type
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
                            result = uniform<std::int32_t>(array, min_value, max_value);
                            break;
                        }
                        case 8:
                        {
                            result = uniform<std::int64_t>(array, min_value, max_value);
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
                        case 4:
                        {
                            result = uniform<std::uint32_t>(array, min_value, max_value);
                            break;
                        }
                        case 8:
                        {
                            result = uniform<std::uint64_t>(array, min_value, max_value);
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
                            result = uniform<float>(array, min_value, max_value);
                            break;
                        }
                        case 8:
                        {
                            result = uniform<double>(array, min_value, max_value);
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


        class Binder
        {

            public:

                template<Arithmetic Element>
                static void bind(pybind11::module& module)
                {
                    Rank const rank{2};

                    module.def(
                        "uniform",
                        uniform<Element, rank>,
                        R"(
    Create new array, filled with uniformly distributed random values

    :param array: "Clone array" to query for array shape and partition shape
            to use
    :param numpy.dtype dtype: Type of the array elements
    :param min_value: Minimum potentially generated value
    :param max_value: Maximum potentially generated value
    :rtype: PartitionedArray specialization

    The type of the array returned depends on the rank of the array and
    the type of the array elements.
)",
                        "array"_a,
                        "dtype"_a,
                        "min_value"_a,
                        "max_value"_a);
                }
        };

    }  // Anonymous namespace


    void bind_uniform1(pybind11::module& module)
    {
        bind<Binder, ArithmeticElements>(module);
    }

}  // namespace lue::framework
