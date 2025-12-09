#pragma once
#include "lue/array/different_shape/value.hpp"
#include "lue/array/value_group.hpp"


namespace lue::data_model::different_shape::variable_shape {

    /*!
        @brief      Class for storing different shape x variable value x variable shape object arrays

        The implementation uses an HDF5 group, containing an HDF5 group for each location in time. Each of
        these groups contains an HDF5 dataset for each active object.
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

            auto nr_locations_in_time() const -> Count;

            auto expand(Index idx, Count nr_objects, ID const* ids, hdf5::Shape const* shapes)
                -> different_shape::Value;

            auto operator[](Index idx) -> different_shape::Value;

        private:

            Count _nr_locations_in_time;
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

}  // namespace lue::data_model::different_shape::variable_shape
