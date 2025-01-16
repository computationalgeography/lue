#include "shape.hpp"
#include "lue/framework/core/domain_decomposition.hpp"
#include "lue/framework/io/read_into.hpp"
#include "lue/framework.hpp"
#include <pybind11/numpy.h>
#include <pybind11/stl.h>


namespace lh5 = lue::hdf5;
namespace ldm = lue::data_model;
using namespace pybind11::literals;


namespace lue::framework {
    namespace {

        auto shape_to_offset(DynamicShape const& shape) -> lh5::Offset
        {
            lh5::Offset result(shape.size());

            for (std::size_t i = 0; i < shape.size(); ++i)
            {
                result[i] = shape[i];
            }

            return result;
        }


        auto shape_to_shape(DynamicShape const& shape) -> lh5::Shape
        {
            lh5::Shape result(shape.size());

            for (std::size_t i = 0; i < shape.size(); ++i)
            {
                result[i] = shape[i];
            }

            return result;
        }


        template<typename ArrayIO, Rank rank>
        auto read(
            ArrayIO const& io,
            lh5::Datatype const& datatype,
            std::string const& array_pathname,
            StaticShape<rank> const& partition_shape,
            ldm::ID const object_id) -> pybind11::object
        {
            pybind11::object result;

            if constexpr (lue::arithmetic_element_supported<std::uint8_t>)
            {
                if (datatype == lh5::std_uint8_le)
                {
                    result = io.template read<std::uint8_t, 2>(array_pathname, partition_shape, object_id);
                }
            }

            if constexpr (lue::arithmetic_element_supported<std::uint32_t>)
            {
                if (datatype == lh5::std_uint32_le)
                {
                    result = io.template read<std::uint32_t, 2>(array_pathname, partition_shape, object_id);
                }
            }

            if constexpr (lue::arithmetic_element_supported<std::int32_t>)
            {
                if (datatype == lh5::std_int32_le)
                {
                    result = io.template read<std::int32_t, 2>(array_pathname, partition_shape, object_id);
                }
            }

            if constexpr (lue::arithmetic_element_supported<std::uint64_t>)
            {
                if (datatype == lh5::std_uint64_le)
                {
                    result = io.template read<std::uint64_t, 2>(array_pathname, partition_shape, object_id);
                }
            }

            if constexpr (lue::arithmetic_element_supported<std::int64_t>)
            {
                if (datatype == lh5::std_int64_le)
                {
                    result = io.template read<std::int64_t, 2>(array_pathname, partition_shape, object_id);
                }
            }

            if constexpr (lue::arithmetic_element_supported<float>)
            {
                if (datatype == lh5::ieee_float32_le)
                {
                    result = io.template read<float, 2>(array_pathname, partition_shape, object_id);
                }
            }

            if constexpr (lue::arithmetic_element_supported<double>)
            {
                if (datatype == lh5::ieee_float64_le)
                {
                    result = io.template read<double, 2>(array_pathname, partition_shape, object_id);
                }
            }

            if (!result)
            {
                // TODO
                throw std::runtime_error(
                    fmt::format("Unsupported datatype: {}", "TODO"));  // as_string(datatype)));
            }

            return result;
        }


        template<typename Element, Rank rank>
        auto read_array(
            std::string const& array_pathname,
            lh5::Hyperslab const& hyperslab,
            StaticShape<rank> const& partition_shape,
            data_model::ID const object_id) -> PartitionedArray<Element, rank>
        {
            using Policies = policy::read_into::DefaultValuePolicies<Element>;

            return hyperslab.empty() ? lue::read<Element, Policies, rank>(
                                           Policies{}, array_pathname, partition_shape, object_id)
                                     : lue::read<Element, Policies, rank>(
                                           Policies{}, array_pathname, hyperslab, partition_shape, object_id);
        }


        template<typename Element, Rank rank>
        auto read_array(
            std::string const& array_pathname,
            StaticShape<rank> const& partition_shape,
            data_model::ID const object_id,
            Index const time_step_idx) -> PartitionedArray<Element, rank>
        {
            using Policies = policy::read_into::DefaultValuePolicies<Element>;

            return lue::read<Element, Policies, rank>(
                Policies{}, array_pathname, partition_shape, object_id, time_step_idx);
        }


        namespace constant {

            class ArrayIO
            {

                public:

                    ArrayIO():

                        _hyperslab{}

                    {
                    }


                    ArrayIO(lh5::Hyperslab const hyperslab):

                        _hyperslab{hyperslab}

                    {
                    }


                    template<typename Element, Rank rank>
                    pybind11::object read(
                        std::string const& array_pathname,
                        StaticShape<rank> const& partition_shape,
                        data_model::ID const object_id) const
                    {
                        return pybind11::cast(read_array<Element, rank>(
                            array_pathname, _hyperslab, partition_shape, object_id));
                    }


                private:

                    lh5::Hyperslab _hyperslab;
            };

        }  // namespace constant


        namespace variable {

            class ArrayIO
            {

                public:

                    ArrayIO(Index const time_step_idx):

                        _time_step_idx{time_step_idx}

                    {
                    }

                    template<typename Element, Rank rank>
                    pybind11::object read(
                        std::string const& array_pathname,
                        StaticShape<rank> const& partition_shape,
                        data_model::ID const object_id) const
                    {
                        return pybind11::cast(read_array<Element, rank>(
                            array_pathname, partition_shape, object_id, _time_step_idx));
                    }

                private:

                    Index _time_step_idx;
            };

        }  // namespace variable


        auto read_array_py1(
            std::string const& array_pathname,
            std::optional<pybind11::tuple> const& partition_shape) -> pybind11::object
        {
            auto const [dataset_pathname, phenomenon_name, property_set_name, layer_name] =
                parse_array_pathname(array_pathname);
            auto const [object_id, dynamic_array_shape, datatype] =
                ldm::constant::probe_raster(dataset_pathname, phenomenon_name, property_set_name, layer_name);

            Rank const rank{2};

            pybind11::object result;

            verify_rank_supported(rank);

            if constexpr (rank_supported(2))
            {
                if (rank == 2)
                {
                    StaticShape<rank> const static_array_shape{
                        dynamic_shape_to_static_shape<2>(dynamic_array_shape)};
                    StaticShape<rank> static_partition_shape{};

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

                        static_partition_shape = dynamic_shape_to_static_shape<rank>(dynamic_partition_shape);
                    }
                    else
                    {
                        static_partition_shape = default_partition_shape(static_array_shape);
                    }

                    result = read(
                        constant::ArrayIO{}, datatype, array_pathname, static_partition_shape, object_id);
                }
            }

            lue_hpx_assert(result);

            return result;
        }


        auto read_array_py2(
            std::string const& array_pathname,
            pybind11::tuple const& center_cell,
            pybind11::tuple const& subset_shape,
            std::optional<pybind11::tuple> const& partition_shape) -> pybind11::object
        {
            DynamicShape const center_cell_{tuple_to_shape(center_cell)};
            DynamicShape const subset_shape_{tuple_to_shape(subset_shape)};

            auto const [dataset_pathname, phenomenon_name, property_set_name, layer_name] =
                parse_array_pathname(array_pathname);
            auto const [object_id, dynamic_array_shape, datatype] =
                ldm::constant::probe_raster(dataset_pathname, phenomenon_name, property_set_name, layer_name);

            Rank const rank{2};

            pybind11::object result;

            verify_rank_supported(rank);

            if (center_cell_.size() != rank)
            {
                throw std::runtime_error(fmt::format(
                    "Rank of array shape and number of center cell indices passed in must be equal ({} != "
                    "{})",
                    rank,
                    center_cell_.size()));
            }

            if (subset_shape_.size() != rank)
            {
                throw std::runtime_error(fmt::format(
                    "Rank of array shape and array shape passed in must be equal ({} != {})",
                    rank,
                    subset_shape_.size()));
            }

            if constexpr (rank_supported(2))
            {
                if (rank == 2)
                {
                    StaticShape<rank> const static_array_shape{
                        dynamic_shape_to_static_shape<rank>(dynamic_array_shape)};
                    StaticShape<rank> static_partition_shape{};

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

                        static_partition_shape = dynamic_shape_to_static_shape<rank>(dynamic_partition_shape);
                    }
                    else
                    {
                        static_partition_shape = default_partition_shape(static_array_shape);
                    }

                    lh5::Hyperslab const hyperslab{lh5::hyperslab(
                        shape_to_offset(center_cell_), shape_to_shape(subset_shape_), dynamic_array_shape)};

                    result = read(
                        constant::ArrayIO{hyperslab},
                        datatype,
                        array_pathname,
                        static_partition_shape,
                        object_id);
                }
            }

            lue_hpx_assert(result);

            return result;
        }


        auto read_array_py3(
            std::string const& array_pathname,
            Index const time_step_idx,
            std::optional<pybind11::tuple> const& partition_shape) -> pybind11::object
        {
            auto const [dataset_pathname, phenomenon_name, property_set_name, layer_name] =
                parse_array_pathname(array_pathname);
            auto const [object_id, dynamic_array_shape, datatype] =
                ldm::variable::probe_raster(dataset_pathname, phenomenon_name, property_set_name, layer_name);

            Rank const rank{2};

            pybind11::object result;

            verify_rank_supported(rank);

            if constexpr (rank_supported(2))
            {
                if (rank == 2)
                {
                    StaticShape<rank> const static_array_shape{
                        dynamic_shape_to_static_shape<rank>(dynamic_array_shape)};
                    StaticShape<rank> static_partition_shape{};

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

                        static_partition_shape = dynamic_shape_to_static_shape<rank>(dynamic_partition_shape);
                    }
                    else
                    {
                        static_partition_shape = default_partition_shape(static_array_shape);
                    }

                    result = read(
                        variable::ArrayIO{time_step_idx},
                        datatype,
                        array_pathname,
                        static_partition_shape,
                        object_id);
                }
            }

            lue_hpx_assert(result);

            return result;
        }

    }  // Anonymous namespace


    void bind_read_array(pybind11::module& module)
    {
        module.def(
            "read_array",
            read_array_py1,
            R"(
    Create new array, filled with values read from dataset

    :param str array_pathname: Pathname to array in dataset
    :param tuple partition_shape: Shape of the array partitions. When not
        passed in, a default shape will be used which might not result in the
        best performance and scalability.
    :rtype: PartitionedArray specialization

    The pathname must be formatted as:
    <dataset pathname>/<phenomenon name>/<property-set name>/<property name>

    It is assumed that the array does not vary through time. Otherwise,
    use the overload that accepts a time step index.
)",
            "array_pathname"_a,
            pybind11::kw_only(),
            "partition_shape"_a = std::optional<pybind11::tuple>{},
            pybind11::return_value_policy::move);


        module.def(
            "read_array",
            read_array_py2,
            R"(
    Create new array, filled with values read from dataset

    :param str array_pathname: Pathname to array in dataset
    :param tuple center_cell: Indices of center cell of array subset
    :param tuple subset_shape: Shape of array subset
    :param tuple partition_shape: Shape of the array partitions. When not
        passed in, a default shape will be used which might not result in the
        best performance and scalability.
    :rtype: PartitionedArray specialization

    The pathname must be formatted as:
    <dataset pathname>/<phenomenon name>/<property-set name>/<property name>

    It is assumed that the array does not vary through time. Otherwise,
    use the overload that accepts a time step index.
)",
            "array_pathname"_a,
            "center_cell"_a,
            "subset_shape"_a,
            pybind11::kw_only(),
            "partition_shape"_a = std::optional<pybind11::tuple>{},
            pybind11::return_value_policy::move);


        module.def(
            "read_array",
            read_array_py3,
            R"(
    Create new array, filled with values read from dataset

    :param str array_pathname: Pathname to array in dataset
    :param int time_step_idx: Index of the time step to read [0, nr_time_steps)
    :param tuple partition_shape: Shape of the array partitions. When not
        passed in, a default shape will be used which might not result in the
        best performance and scalability.
    :rtype: PartitionedArray specialization

    The pathname must be formatted as:
    <dataset pathname>/<phenomenon name>/<property-set name>/<property name>

    It is assumed that the array varies through time. Otherwise,
    use the overload without a time step index.
)",
            "array_pathname"_a,
            "time_step_idx"_a,
            pybind11::kw_only(),
            "partition_shape"_a = std::optional<pybind11::tuple>{},
            pybind11::return_value_policy::move);
    }

}  // namespace lue::framework
