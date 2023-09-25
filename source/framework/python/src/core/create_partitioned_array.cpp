#include "lue/framework/algorithm/create_partitioned_array.hpp"
#include "shape.hpp"
#include <pybind11/numpy.h>
#include <pybind11/stl.h>


using namespace pybind11::literals;


namespace lue::framework {
    namespace {

        template<typename Element, Rank rank>
        PartitionedArray<Element, rank> create_array(
            ShapeT<PartitionedArray<Element, rank>> const& array_shape,
            ShapeT<PartitionedArray<Element, rank>> const& partition_shape,
            pybind11::object const& fill_value)
        {
            using Policies = lue::policy::create_partitioned_array::DefaultValuePolicies<Element>;
            using Functor = lue::InstantiateFilled<Element, rank>;

            return lue::create_partitioned_array(
                Policies{}, array_shape, partition_shape, Functor{pybind11::cast<Element>(fill_value)});
        }


        template<Rank rank>
        pybind11::object create_array(
            StaticShape<rank> const& array_shape,
            StaticShape<rank> const& partition_shape,
            pybind11::dtype const& dtype,
            pybind11::object const& fill_value)
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
                            result = pybind11::cast(
                                create_array<std::int32_t, rank>(array_shape, partition_shape, fill_value));
                            break;
                        }
                        case 8:
                        {
                            result = pybind11::cast(
                                create_array<std::int64_t, rank>(array_shape, partition_shape, fill_value));
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
                            result = pybind11::cast(
                                create_array<std::uint8_t, rank>(array_shape, partition_shape, fill_value));
                            break;
                        }
                        case 4:
                        {
                            result = pybind11::cast(
                                create_array<std::uint32_t, rank>(array_shape, partition_shape, fill_value));
                            break;
                        }
                        case 8:
                        {
                            result = pybind11::cast(
                                create_array<std::uint64_t, rank>(array_shape, partition_shape, fill_value));
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
                            result = pybind11::cast(
                                create_array<float, rank>(array_shape, partition_shape, fill_value));
                            break;
                        }
                        case 8:
                        {
                            result = pybind11::cast(
                                create_array<double, rank>(array_shape, partition_shape, fill_value));
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


        pybind11::object create_array_py(
            pybind11::tuple const& array_shape,
            pybind11::object const& dtype_args,
            pybind11::object const& fill_value,
            std::optional<pybind11::tuple> const& partition_shape)
        {
            DynamicShape const dynamic_array_shape{tuple_to_shape(array_shape)};
            pybind11::dtype const dtype{pybind11::dtype::from_args(dtype_args)};
            Rank const rank{dynamic_array_shape.size()};

            if (partition_shape)
            {
                DynamicShape const dynamic_partition_shape{tuple_to_shape(*partition_shape)};

                if (dynamic_array_shape.size() != dynamic_partition_shape.size())
                {
                    throw std::runtime_error(fmt::format(
                        "Rank of array shape and partition shape must be equal ({} != {})",
                        dynamic_array_shape.size(),
                        dynamic_partition_shape.size()));
                }
            }

            pybind11::object result;

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

                result = create_array<1>(static_array_shape, static_partition_shape, dtype, fill_value);
            }
            else if (rank == 2)
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

                result = create_array<2>(static_array_shape, static_partition_shape, dtype, fill_value);
            }
            else
            {
                throw std::runtime_error("Currently only arrays of rank 1 or 2 are supported");
            }

            lue_hpx_assert(result);

            return result;
        }

    }  // Anonymous namespace


    void bind_create_array(pybind11::module& module)
    {
        module.def(
            "create_array",
            create_array_py,
            R"(
    Create new array, filled with a value

    :param tuple array_shape: Shape of the array
    :param numpy.dtype dtype: Type of the array elements
    :param fill_value: Value to fill array with
    :param tuple partition_shape: Shape of the array partitions. When not
        passed in, a default shape will be used which might not result in the
        best performance and scalability.
    :rtype: PartitionedArray specialization

    The type of the array returned depends on the rank of the array and
    the type of the array elements.
)",
            "array_shape"_a,
            "dtype"_a,
            "fill_value"_a,
            pybind11::kw_only(),
            "partition_shape"_a = std::optional<pybind11::tuple>{},
            pybind11::return_value_policy::move);
    }

}  // namespace lue::framework
