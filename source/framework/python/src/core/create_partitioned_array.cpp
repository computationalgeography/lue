#include "lue/framework/core/component/partitioned_array.hpp"
#include <pybind11/numpy.h>


namespace lue {
namespace framework {
namespace {

using DynamicShape = std::vector<lue::Count>;

template<
    Rank rank>
using StaticShape = std::array<lue::Count, rank>;


DynamicShape tuple_to_shape(
    pybind11::tuple const& tuple)
{
    Rank rank{tuple.size()};
    DynamicShape shape(rank);

    for(std::size_t d = 0; d < tuple.size(); ++d)
    {
        shape[d] = pybind11::int_(tuple[d]);
    }

    return shape;
}


template<
    typename Element,
    Rank rank>
PartitionedArray<Element, rank> create_partitioned_array(
    ShapeT<PartitionedArray<Element, rank>> const& array_shape,
    ShapeT<PartitionedArray<Element, rank>> const& partition_shape,
    pybind11::object const& fill_value)
{
    // TODO Check that this works asynchronous in Release mode. In Debug
    //     mode it doesn't (yet), because of validation.
    using Array = PartitionedArray<Element, rank>;

    return Array{array_shape, partition_shape, pybind11::cast<Element>(fill_value), Array::ClampMode::merge};
}


template<
    Rank rank,
    typename Count>
std::array<Count, rank> dynamic_shape_to_static_shape(
    std::vector<Count> const& shape)
{
    lue_assert(shape.size() == rank);
    std::array<Count, rank> result{};
    std::copy(shape.begin(), shape.end(), result.begin());
    return result;
}


template<
    Rank rank>
pybind11::object create_partitioned_array(
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

    switch(kind)
    {
        case 'i':
        {
            // Signed integer
            switch(size)
            {
                case 4: {
                    result = pybind11::cast(create_partitioned_array<std::int32_t, rank>(
                        array_shape, partition_shape, fill_value));
                    break;
                }
                case 8: {
                    result = pybind11::cast(create_partitioned_array<std::int64_t, rank>(
                        array_shape, partition_shape, fill_value));
                    break;
                }
            }

            break;
        }
        case 'u':
        {
            // Unsigned integer
            switch(size)
            {
                case 1: {
                    result = pybind11::cast(create_partitioned_array<std::uint8_t, rank>(
                        array_shape, partition_shape, fill_value));
                    break;
                }
                case 4: {
                    result = pybind11::cast(create_partitioned_array<std::uint32_t, rank>(
                        array_shape, partition_shape, fill_value));
                    break;
                }
                case 8: {
                    result = pybind11::cast(create_partitioned_array<std::uint64_t, rank>(
                        array_shape, partition_shape, fill_value));
                    break;
                }
            }

            break;
        }
        case 'f':
        {
            // Floating-point
            switch(size)
            {
                case 4: {
                    result = pybind11::cast(create_partitioned_array<float, rank>(
                        array_shape, partition_shape, fill_value));
                    break;
                }
                case 8: {
                    result = pybind11::cast(create_partitioned_array<double, rank>(
                        array_shape, partition_shape, fill_value));
                    break;
                }
            }

            break;
        }
    }

    if(!result)
    {
        throw std::runtime_error(fmt::format("Unsupported dtype (kind={}, itemsize={})", kind, size));
    }

    return result;
}


pybind11::object create_partitioned_array(
    DynamicShape const& array_shape,
    DynamicShape const& partition_shape,
    pybind11::dtype const& dtype,
    pybind11::object const& fill_value)
{
    if(array_shape.size() != partition_shape.size())
    {
        throw std::runtime_error(fmt::format(
            "Rank of array shape and partition shape must be equal ({} != {})",
            array_shape.size(), partition_shape.size()));
    }

    Rank const rank{array_shape.size()};
    pybind11::object result;

    if(rank == 1)
    {
        result = create_partitioned_array<1>(
            dynamic_shape_to_static_shape<1>(array_shape),
            dynamic_shape_to_static_shape<1>(partition_shape),
            dtype, fill_value);
    }
    else if(rank == 2)
    {
        result = create_partitioned_array<2>(
            dynamic_shape_to_static_shape<2>(array_shape),
            dynamic_shape_to_static_shape<2>(partition_shape),
            dtype, fill_value);
    }
    else
    {
        throw std::runtime_error("Currently only arrays of rank 1 or 2 are supported");
    }

    lue_assert(result);

    return result;
}

}  // Anonymous namespace


void bind_create_partitioned_array(
    pybind11::module& module)
{
    module.def(
        "create_partitioned_array",
        [](
            pybind11::tuple const& array_shape,
            pybind11::tuple const& partition_shape,
            pybind11::dtype const& dtype,
            pybind11::object const& fill_value)
        {
            return create_partitioned_array(
                tuple_to_shape(array_shape), tuple_to_shape(partition_shape), dtype, fill_value);
        },
        pybind11::return_value_policy::move);
}

}  // namespace framework
}  // namespace lue
