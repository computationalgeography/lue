#include "lue/array/different_shape/variable_shape/value.hpp"
#include "lue/core/tag.hpp"


namespace lue {
    namespace data_model {
        namespace different_shape {
            namespace variable_shape {

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
                Value::Value(
                    hdf5::Group const& parent,
                    std::string const& name,
                    hdf5::Datatype const& memory_datatype):

                    ValueGroup{parent, name, memory_datatype},
                    _nr_locations_in_time{attributes().read<Count>(nr_locations_in_time_tag)}

                {
                }


                Value::Value(ValueGroup&& group):

                    ValueGroup{std::move(group)},
                    _nr_locations_in_time{attributes().read<Count>(nr_locations_in_time_tag)}

                {
                }


                Count Value::nr_locations_in_time() const
                {
                    return _nr_locations_in_time;
                }


                /*!
                    @brief      Make space for additional object arrays of objects
                    @param      idx Index of location in time
                    @param      nr_objects Number of objects (equals number of object arrays)
                    @param      ids For each object, the object ID
                    @param      shapes For each object, the shape of the object array
                    @return     An instance representing the collection of object
                                arrays. It can be used to write object information.
                */
                different_shape::Value Value::expand(
                    Index const idx, Count const nr_objects, ID const* ids, hdf5::Shape const* shapes)
                {
                    std::string const name{std::to_string(idx)};
                    different_shape::Value value{different_shape::create_value(
                        *this, name, file_datatype(), memory_datatype(), rank())};

                    value.expand(nr_objects, ids, shapes);

                    attributes().write<Count>(nr_locations_in_time_tag, ++_nr_locations_in_time);

                    return value;
                }


                different_shape::Value Value::operator[](Index const idx)
                {
                    std::string const name{std::to_string(idx)};

                    return different_shape::Value{*this, name, memory_datatype()};
                }


                /*!
                    @brief      Create value @a name in @a parent
                */
                Value create_value(
                    hdf5::Group& parent,
                    std::string const& name,
                    hdf5::Datatype const& memory_datatype,
                    Rank const rank)
                {
                    return create_value(parent, name, file_datatype(memory_datatype), memory_datatype, rank);
                }


                /*!
                    @brief      Create value @a name in @a parent
                */
                Value create_value(
                    hdf5::Group& parent,
                    std::string const& name,
                    hdf5::Datatype const& file_datatype,
                    hdf5::Datatype const& memory_datatype,
                    Rank const rank)
                {
                    ValueGroup group{create_value_group(parent, name, file_datatype, memory_datatype, rank)};

                    group.attributes().write<Count>(nr_locations_in_time_tag, 0);

                    return Value{std::move(group)};
                }

            }  // namespace variable_shape
        }      // namespace different_shape
    }          // namespace data_model
}  // namespace lue
