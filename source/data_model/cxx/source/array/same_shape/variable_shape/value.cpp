#include "lue/array/same_shape/variable_shape/value.hpp"
#include "lue/core/tag.hpp"


namespace lue::data_model::same_shape::variable_shape {

    /*!
        @brief      Open value @a name in @a parent
    */
    Value::Value(hdf5::Group const& parent, std::string const& name):

        ValueGroup{parent, name},
        _nr_locations_in_time{attributes().read<Count>(nr_locations_in_time_tag)}

    {
    }


    /*!
        @brief      Open value @a name in @a parent
    */
    Value::Value(hdf5::Group const& parent, std::string const& name, hdf5::Datatype const& memory_datatype):

        ValueGroup{parent, name, memory_datatype},
        _nr_locations_in_time{attributes().read<Count>(nr_locations_in_time_tag)}

    {
    }


    Value::Value(ValueGroup&& group):

        ValueGroup{std::move(group)},
        _nr_locations_in_time{attributes().read<Count>(nr_locations_in_time_tag)}

    {
    }


    auto Value::nr_locations_in_time() const -> Count
    {
        return _nr_locations_in_time;
    }


    /*!
        @brief      Make space for an additional number of object arrays
        @param      idx Index of location in time
        @param      nr_arrays Number of arrays
        @param      array_shape Shape of an object array
        @return     The created value representing the object arrays for the location in time. It can be used
                    to write the object information.
    */
    auto Value::expand(Index const idx, Count const nr_arrays, hdf5::Shape const& array_shape)
        -> same_shape::Value
    {
        std::string const name{std::to_string(idx)};
        same_shape::Value value{
            same_shape::create_value(*this, name, file_datatype(), memory_datatype(), array_shape)};
        value.expand(nr_arrays);
        attributes().write<Count>(nr_locations_in_time_tag, ++_nr_locations_in_time);

        return value;
    }


    auto Value::operator[](Index const idx) -> same_shape::Value
    {
        std::string const name{std::to_string(idx)};

        return same_shape::Value{*this, name};
    }


    /*!
        @brief      Create value @a name in @a parent
    */
    auto create_value(
        hdf5::Group& parent, std::string const& name, hdf5::Datatype const& memory_datatype, Rank const rank)
        -> Value
    {
        return create_value(parent, name, file_datatype(memory_datatype), memory_datatype, rank);
    }


    /*!
        @brief      Create value @a name in @a parent
    */
    auto create_value(
        hdf5::Group& parent,
        std::string const& name,
        hdf5::Datatype const& file_datatype,
        hdf5::Datatype const& memory_datatype,
        Rank const rank) -> Value
    {
        ValueGroup group{create_value_group(parent, name, file_datatype, memory_datatype, rank)};

        group.attributes().write<Count>(nr_locations_in_time_tag, 0);

        return Value{std::move(group)};
    }

}  // namespace lue::data_model::same_shape::variable_shape
