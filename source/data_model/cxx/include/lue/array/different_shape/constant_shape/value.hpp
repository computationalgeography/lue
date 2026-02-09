#pragma once
#include "lue/array/same_shape/constant_shape/value.hpp"
#include "lue/array/value_group.hpp"


namespace lue::data_model::different_shape::constant_shape {

    /*!
        @brief      Class for storing different shape x constant shape object arrays

        The implementation uses an HDF5 dataset per object. This dataset has one dimension more than the rank
        of the object arrays. This first dimension represents the time. The first location in time for which
        an object array is stored for an object is stored at the first index in the object's corresponding
        HDF5 dataset, etc.

        As an example, when storing stacks of rasters for one or more time boxes, one way to do that is to
        store a 3D object array per time box and discretize this value using time and space discretization
        properties. To read or write a whole singe raster from the HDF5 dataset you need to setup a hyperslab
        as follows:

        - offset:
            - time box index [0, nr_time_boxes)
            - time step index [0, nr_time_steps)
            - 0
            - 0
        - count:
            - 1
            - 1
            - number of rows
            - number of columns
    */
    class LUE_DATA_MODEL_EXPORT Value: public ValueGroup
    {

        public:

            Value(hdf5::Group const& parent, std::string const& name);

            Value(hdf5::Group const& parent, std::string const& name, hdf5::Datatype const& memory_datatype);

            explicit Value(ValueGroup&& group);

            Value(Value const& other) = default;

            Value(Value&& other) = default;

            ~Value() override = default;

            auto operator=(Value const& other) -> Value& = default;

            auto operator=(Value&& other) -> Value& = default;

            auto nr_objects() const -> Count;

            auto exists(ID id) const -> bool;

            void expand(
                Count nr_objects,
                ID const* ids,
                hdf5::Shape const* shapes,
                Count const* nr_locations_in_time);

            auto expand(ID id, hdf5::Shape const& shape, Count nr_locations_in_time)
                -> same_shape::constant_shape::Value;

            auto operator[](ID id) const -> same_shape::constant_shape::Value;

        private:

            auto expand_(ID id, hdf5::Shape const& shape, Count nr_locations_in_time)
                -> same_shape::constant_shape::Value;

            Count _nr_objects;
    };


    LUE_DATA_MODEL_EXPORT auto create_value(
        hdf5::Group& parent, std::string const& name, hdf5::Datatype const& memory_datatype, Rank rank)
        -> Value;

    LUE_DATA_MODEL_EXPORT auto create_value(
        hdf5::Group& parent,
        std::string const& name,
        hdf5::Datatype const& file_datatype,
        hdf5::Datatype const& memory_datatype,
        Rank rank) -> Value;

}  // namespace lue::data_model::different_shape::constant_shape
