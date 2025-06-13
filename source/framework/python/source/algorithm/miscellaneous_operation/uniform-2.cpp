#include "shape.hpp"
#include "lue/framework/algorithm/value_policies/uniform.hpp"
#include "lue/framework/core/domain_decomposition.hpp"
#include "lue/framework.hpp"
#include <pybind11/numpy.h>
#include <pybind11/stl.h>


using namespace pybind11::literals;


namespace lue::framework {
    namespace {

        // Step 3: Call the algorithm
        template<typename Element, Rank rank>
        auto uniform(
            StaticShape<rank> const& array_shape,
            StaticShape<rank> const& partition_shape,
            pybind11::object const& min_value,
            pybind11::object const& max_value) -> pybind11::object
        {
            pybind11::object result{};

            if constexpr (arithmetic_element_supported<Element>)
            {
                result = pybind11::cast(
                    value_policies::uniform(
                        array_shape,
                        partition_shape,
                        pybind11::cast<Element>(min_value),
                        pybind11::cast<Element>(max_value)));
            }

            return result;
        }


        // Step 2: Determine the type of the element type of the result array
        template<Rank rank>
        auto uniform(
            StaticShape<rank> const& array_shape,
            StaticShape<rank> const& partition_shape,
            pybind11::dtype const& dtype,
            pybind11::object const& min_value,
            pybind11::object const& max_value) -> pybind11::object
        {
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
                            result =
                                uniform<std::int32_t>(array_shape, partition_shape, min_value, max_value);
                            break;
                        }
                        case 8:
                        {
                            result =
                                uniform<std::int64_t>(array_shape, partition_shape, min_value, max_value);
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
                            result =
                                uniform<std::uint32_t>(array_shape, partition_shape, min_value, max_value);
                            break;
                        }
                        case 8:
                        {
                            result =
                                uniform<std::uint64_t>(array_shape, partition_shape, min_value, max_value);
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
                            result = uniform<float>(array_shape, partition_shape, min_value, max_value);
                            break;
                        }
                        case 8:
                        {
                            result = uniform<double>(array_shape, partition_shape, min_value, max_value);
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


        // Step 1: Convert from Python types to C++ types and validate contents
        auto uniform_py(
            pybind11::tuple const& array_shape,
            pybind11::object const& dtype_args,
            pybind11::object const& min_value,
            pybind11::object const& max_value,
            std::optional<pybind11::tuple> const& partition_shape) -> pybind11::object
        {
            DynamicShape const dynamic_array_shape{tuple_to_shape(array_shape)};
            pybind11::dtype const dtype{pybind11::dtype::from_args(dtype_args)};
            Rank const rank{dynamic_array_shape.size()};

            if (partition_shape)
            {
                DynamicShape const dynamic_partition_shape{tuple_to_shape(*partition_shape)};

                if (dynamic_array_shape.size() != dynamic_partition_shape.size())
                {
                    throw std::runtime_error(
                        std::format(
                            "Rank of array shape and partition shape must be equal ({} != {})",
                            dynamic_array_shape.size(),
                            dynamic_partition_shape.size()));
                }
            }

            pybind11::object result;

            verify_rank_supported(rank);

            if constexpr (rank_supported(1))
            {
                if (rank == 1)
                {
                    StaticShape<1> const static_array_shape{
                        dynamic_shape_to_static_shape<1>(dynamic_array_shape)};
                    StaticShape<1> static_partition_shape{};

                    if (partition_shape)
                    {
                        static_partition_shape =
                            dynamic_shape_to_static_shape<1>(tuple_to_shape(*partition_shape));
                    }
                    else
                    {
                        static_partition_shape = default_partition_shape(static_array_shape);
                    }

                    result =
                        uniform<1>(static_array_shape, static_partition_shape, dtype, min_value, max_value);
                }
            }

            if constexpr (rank_supported(2))
            {
                if (rank == 2)
                {
                    StaticShape<2> const static_array_shape{
                        dynamic_shape_to_static_shape<2>(dynamic_array_shape)};
                    StaticShape<2> static_partition_shape{};

                    if (partition_shape)
                    {
                        static_partition_shape =
                            dynamic_shape_to_static_shape<2>(tuple_to_shape(*partition_shape));
                    }
                    else
                    {
                        static_partition_shape = default_partition_shape(static_array_shape);
                    }

                    result =
                        uniform<2>(static_array_shape, static_partition_shape, dtype, min_value, max_value);
                }
            }

            lue_hpx_assert(result);

            return result;
        }

    }  // Anonymous namespace


    void bind_uniform2(pybind11::module& module)
    {
        module.def(
            "uniform",
            uniform_py,
            R"(
    Create new array, filled with uniformly distributed random values

    :param tuple array_shape: Shape of the array
    :param numpy.dtype dtype: Type of the array elements
    :param min_value: Minimum potentially generated value
    :param max_value: Maximum potentially generated value
    :param tuple partition_shape: Shape of the array partitions. When not
        passed in, a default shape will be used which might not result in the
        best performance and scalability.
    :rtype: PartitionedArray specialization

    The type of the array returned depends on the rank of the array and
    the type of the array elements.
)",
            "array_shape"_a,
            "dtype"_a,
            "min_value"_a,
            "max_value"_a,
            pybind11::kw_only(),
            "partition_shape"_a = std::optional<pybind11::tuple>{},
            pybind11::return_value_policy::move);
    }

}  // namespace lue::framework
