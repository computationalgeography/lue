#include "lue/framework/algorithm/value_policies/unique_id.hpp"
#include <pybind11/numpy.h>


using namespace pybind11::literals;


namespace lue::framework {
    namespace {

        Rank const rank{2};

        template<typename ConditionElement>
        pybind11::object unique_id(
            PartitionedArray<ConditionElement, rank> const& condition, pybind11::object const& dtype_args)
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
                            result = pybind11::cast(value_policies::unique_id<std::int32_t>(condition));
                            break;
                        }
                        case 8:
                        {
                            result = pybind11::cast(value_policies::unique_id<std::int64_t>(condition));
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
                            result = pybind11::cast(value_policies::unique_id<std::uint8_t>(condition));
                            break;
                        }
                        case 4:
                        {
                            result = pybind11::cast(value_policies::unique_id<std::uint32_t>(condition));
                            break;
                        }
                        case 8:
                        {
                            result = pybind11::cast(value_policies::unique_id<std::uint64_t>(condition));
                            break;
                        }
                    }

                    break;
                }
            }

            if (!result)
            {
                throw std::runtime_error(fmt::format(
                    "Operation expects dtype representing uint8, uint{{32,64}}, or int{{32,64}}, "
                    "but got: kind={}, itemsize={}",
                    kind,
                    size));
            }

            return result;
        }

    }  // Anonymous namespace


    void bind_unique_id(pybind11::module& module)
    {
        module.def("unique_id", unique_id<std::uint8_t>, "condition"_a, "dtype"_a);
    }

}  // namespace lue::framework
