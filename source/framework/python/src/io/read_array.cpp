#include "shape.hpp"
#include "lue/framework/io/read_into.hpp"
#include <pybind11/numpy.h>


namespace lh5 = lue::hdf5;
namespace ldm = lue::data_model;
using namespace pybind11::literals;


namespace lue::framework {
    namespace {

        lh5::Offset shape_to_offset(DynamicShape const& shape)
        {
            lh5::Offset result(shape.size());

            for (std::size_t i = 0; i < shape.size(); ++i)
            {
                result[i] = shape[i];
            }

            return result;
        }


        lh5::Shape shape_to_shape(DynamicShape const& shape)
        {
            lh5::Shape result(shape.size());

            for (std::size_t i = 0; i < shape.size(); ++i)
            {
                result[i] = shape[i];
            }

            return result;
        }


        template<typename ArrayIO, Rank rank>
        pybind11::object read(
            ArrayIO const& io,
            lh5::Datatype const& datatype,
            std::string const& array_pathname,
            StaticShape<rank> const& partition_shape,
            ldm::ID const object_id)
        {
            pybind11::object result;

            if (datatype == lh5::std_uint8_le)
            {
                result = io.template read<std::uint8_t, 2>(array_pathname, partition_shape, object_id);
            }
            else if (datatype == lh5::std_uint32_le)
            {
                result = io.template read<std::uint32_t, 2>(array_pathname, partition_shape, object_id);
            }
            else if (datatype == lh5::std_int32_le)
            {
                result = io.template read<std::int32_t, 2>(array_pathname, partition_shape, object_id);
            }
            else if (datatype == lh5::std_uint64_le)
            {
                result = io.template read<std::uint64_t, 2>(array_pathname, partition_shape, object_id);
            }
            else if (datatype == lh5::std_int64_le)
            {
                result = io.template read<std::int64_t, 2>(array_pathname, partition_shape, object_id);
            }
            else if (datatype == lh5::ieee_float32_le)
            {
                result = io.template read<float, 2>(array_pathname, partition_shape, object_id);
            }
            else if (datatype == lh5::ieee_float64_le)
            {
                result = io.template read<double, 2>(array_pathname, partition_shape, object_id);
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
        PartitionedArray<Element, rank> read_array(
            std::string const& array_pathname,
            lh5::Hyperslab const& hyperslab,
            StaticShape<rank> const& partition_shape,
            data_model::ID const object_id)
        {
            using Policies = policy::read_into::DefaultValuePolicies<Element>;

            return hyperslab.empty() ? lue::read<Element, Policies, rank>(
                                           Policies{}, array_pathname, partition_shape, object_id)
                                     : lue::read<Element, Policies, rank>(
                                           Policies{}, array_pathname, hyperslab, partition_shape, object_id);
        }


        // template<
        //     typename Element,
        //     Rank rank>
        // PartitionedArray<Element, rank> read_array(
        //     std::string const& array_pathname,
        //     StaticShape<rank> const& center_cell,
        //     StaticShape<rank> const& subset_shape,
        //     StaticShape<rank> const& partition_shape,
        //     data_model::ID const object_id)
        // {
        //     using Policies = policy::read_into::DefaultValuePolicies<Element>;

        //     return lue::read<Element, Policies, rank>(
        //         Policies{}, array_pathname, center_cell, subset_shape, partition_shape, object_id);
        // }


        template<typename Element, Rank rank>
        PartitionedArray<Element, rank> read_array(
            std::string const& array_pathname,
            StaticShape<rank> const& partition_shape,
            data_model::ID const object_id,
            Index const time_step_idx)
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


        pybind11::object read_array(
            std::string const& array_pathname,
            DynamicShape const& center_cell,
            DynamicShape const& subset_shape,
            DynamicShape const& partition_shape)
        {
            auto const [dataset_pathname, phenomenon_name, property_set_name, layer_name] =
                parse_array_pathname(array_pathname);
            auto const [object_id, raster_shape, datatype] =
                ldm::constant::probe_raster(dataset_pathname, phenomenon_name, property_set_name, layer_name);

            Rank const rank{2};

            if (rank != 2)
            {
                throw std::runtime_error(
                    fmt::format("Unsupported rank ({}). Currently only rank 2 is supported", rank));
            }

            if (center_cell.size() != rank)
            {
                throw std::runtime_error(fmt::format(
                    "Rank of array shape and number of center cell indices passed in must be equal ({} != "
                    "{})",
                    rank,
                    center_cell.size()));
            }

            if (subset_shape.size() != rank)
            {
                throw std::runtime_error(fmt::format(
                    "Rank of array shape and array shape passed in must be equal ({} != {})",
                    rank,
                    subset_shape.size()));
            }

            if (partition_shape.size() != rank)
            {
                throw std::runtime_error(fmt::format(
                    "Rank of array shape and partition shape passed in must be equal ({} != {})",
                    rank,
                    partition_shape.size()));
            }

            lh5::Hyperslab const hyperslab{
                lh5::hyperslab(shape_to_offset(center_cell), shape_to_shape(subset_shape), raster_shape)};

            return read(
                constant::ArrayIO{hyperslab},
                datatype,
                array_pathname,
                dynamic_shape_to_static_shape<2>(partition_shape),
                object_id);
        }


        pybind11::object read_array(std::string const& array_pathname, DynamicShape const& partition_shape)
        {
            auto const [dataset_pathname, phenomenon_name, property_set_name, layer_name] =
                parse_array_pathname(array_pathname);
            auto const [object_id, _, datatype] =
                ldm::constant::probe_raster(dataset_pathname, phenomenon_name, property_set_name, layer_name);

            Rank const rank{2};

            if (rank != 2)
            {
                throw std::runtime_error(
                    fmt::format("Unsupported rank ({}). Currently only rank 2 is supported", rank));
            }

            if (partition_shape.size() != rank)
            {
                throw std::runtime_error(fmt::format(
                    "Rank of array shape and partition shape must be equal ({} != {})",
                    rank,
                    partition_shape.size()));
            }

            return read(
                constant::ArrayIO{},
                datatype,
                array_pathname,
                dynamic_shape_to_static_shape<2>(partition_shape),
                object_id);
        }


        pybind11::object read_array(
            std::string const& array_pathname, DynamicShape const& partition_shape, Index const time_step_idx)
        {
            auto const [dataset_pathname, phenomenon_name, property_set_name, layer_name] =
                parse_array_pathname(array_pathname);
            auto const [object_id, _, datatype] =
                ldm::variable::probe_raster(dataset_pathname, phenomenon_name, property_set_name, layer_name);

            Rank const rank{2};

            if (rank != 2)
            {
                throw std::runtime_error(
                    fmt::format("Unsupported rank ({}). Currently only rank 2 is supported", rank));
            }

            if (partition_shape.size() != rank)
            {
                throw std::runtime_error(fmt::format(
                    "Rank of array shape and partition shape must be equal ({} != {})",
                    rank,
                    partition_shape.size()));
            }

            return read(
                variable::ArrayIO{time_step_idx},
                datatype,
                array_pathname,
                dynamic_shape_to_static_shape<2>(partition_shape),
                object_id);
        }

    }  // Anonymous namespace


    void bind_read_array(pybind11::module& module)
    {
        module.def(
            "read_array",
            [](std::string const& array_pathname, pybind11::tuple const& partition_shape)
            { return read_array(array_pathname, tuple_to_shape(partition_shape)); },
            R"(
    Read array from dataset

    :param str array_pathname: Pathname to array in dataset
    :param tuple partition_shape: Shape of the array partitions
    :rtype: PartitionedArray specialization

    The pathname must be formatted as:
    <dataset pathname>/<phenomenon name>/<property-set name>/<property name>

    It is assumed that the array does not vary through time. Otherwise,
    use the overload that accepts a time step index.
)",
            "array_pathname"_a,
            "partition_shape"_a)

            ;

        module.def(
            "read_array",
            [](std::string const& array_pathname,
               pybind11::tuple const& center_cell,
               pybind11::tuple const& subset_shape,
               pybind11::tuple const& partition_shape)
            {
                return read_array(
                    array_pathname,
                    tuple_to_shape(center_cell),
                    tuple_to_shape(subset_shape),
                    tuple_to_shape(partition_shape));
            },
            R"(
    Read subset of array from dataset

    :param str array_pathname: Pathname to array in dataset
    :param tuple center_cell: Indices of center cell of array subset
    :param tuple subset_shape: Shape of array subset
    :param tuple partition_shape: Shape of the array partitions
    :rtype: PartitionedArray specialization

    The pathname must be formatted as:
    <dataset pathname>/<phenomenon name>/<property-set name>/<property name>

    It is assumed that the array does not vary through time. Otherwise,
    use the overload that accepts a time step index.
)",
            "array_pathname"_a,
            "center_cell"_a,
            "subset_shape"_a,
            "partition_shape"_a)

            ;


        module.def(
            "read_array",
            [](std::string const& array_pathname,
               pybind11::tuple const& partition_shape,
               Index const time_step_idx)
            { return read_array(array_pathname, tuple_to_shape(partition_shape), time_step_idx); },
            R"(
    Read array from dataset

    :param str array_pathname: Pathname to array in dataset
    :param tuple partition_shape: Shape of the array partitions
    :param int time_step_idx: Index of the time step to read [0, nr_time_steps)
    :rtype: PartitionedArray specialization

    The pathname must be formatted as:
    <dataset pathname>/<phenomenon name>/<property-set name>/<property name>

    It is assumed that the array varies through time. Otherwise,
    use the overload without a time step index.
)",
            "array_pathname"_a,
            "partition_shape"_a,
            "time_step_idx"_a)

            ;
    }

}  // namespace lue::framework
