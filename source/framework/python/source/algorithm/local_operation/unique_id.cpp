#include "lue/framework/algorithm/value_policies/unique_id.hpp"
#include "lue/framework.hpp"
#include <pybind11/numpy.h>


using namespace pybind11::literals;


namespace lue::framework {
    namespace {

        Rank const rank{2};

        auto unique_id(
            PartitionedArray<BooleanElement, rank> const& condition,
            pybind11::object const& dtype_args) -> pybind11::object
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
                        case 1:
                        {
                            using Element = std::int8_t;

                            if constexpr (arithmetic_element_supported<Element>)
                            {
                                result = pybind11::cast(value_policies::unique_id<Element>(condition));
                            }

                            break;
                        }
                        case 4:
                        {
                            using Element = std::int32_t;

                            if constexpr (arithmetic_element_supported<Element>)
                            {
                                result = pybind11::cast(value_policies::unique_id<Element>(condition));
                            }

                            break;
                        }
                        case 8:
                        {
                            using Element = std::int64_t;

                            if constexpr (arithmetic_element_supported<Element>)
                            {
                                result = pybind11::cast(value_policies::unique_id<Element>(condition));
                            }

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
                            using Element = std::uint8_t;

                            if constexpr (arithmetic_element_supported<Element>)
                            {
                                result = pybind11::cast(value_policies::unique_id<Element>(condition));
                            }

                            break;
                        }
                        case 4:
                        {
                            using Element = std::uint32_t;

                            if constexpr (arithmetic_element_supported<Element>)
                            {
                                result = pybind11::cast(value_policies::unique_id<Element>(condition));
                            }

                            break;
                        }
                        case 8:
                        {
                            using Element = std::uint64_t;

                            if constexpr (arithmetic_element_supported<Element>)
                            {
                                result = pybind11::cast(value_policies::unique_id<Element>(condition));
                            }

                            break;
                        }
                    }

                    break;
                }
            }

            if (!result)
            {
                throw std::runtime_error(std::format("Unsupported dtype (kind={}, itemsize={})", kind, size));
            }

            return result;
        }

    }  // Anonymous namespace


    void bind_unique_id(pybind11::module& module)
    {
        module.def("unique_id", unique_id, "condition"_a, "dtype"_a);
    }

}  // namespace lue::framework
