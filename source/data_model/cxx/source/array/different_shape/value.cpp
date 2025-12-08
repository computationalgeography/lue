#include "lue/array/different_shape/value.hpp"
#include "lue/core/tag.hpp"


namespace lue::data_model::different_shape {

    /*!
        @brief      Open value @a name in @a parent
    */
    Value::Value(hdf5::Group& parent, std::string const& name):

        ValueGroup{parent, name},
        _nr_objects{attributes().read<Count>(nr_objects_tag)}

    {
    }


    /*!
        @brief      Open value @a name in @a parent
    */
    Value::Value(hdf5::Group& parent, std::string const& name, hdf5::Datatype const& memory_datatype):

        ValueGroup{parent, name, memory_datatype},
        _nr_objects{attributes().read<Count>(nr_objects_tag)}

    {
    }


    Value::Value(ValueGroup&& group):

        ValueGroup{std::move(group)},
        _nr_objects{attributes().read<Count>(nr_objects_tag)}

    {
    }


    auto Value::nr_objects() const -> Count
    {
        return _nr_objects;
    }


    /*!
        @brief      Make space for additional object arrays of objects
        @param      nr_objects Number of objects (equals number of object arrays and number of value arrays)
        @param      ids For each object, the object ID
        @param      shapes For each object, the shape of the object array
    */
    void Value::expand(Count const nr_objects, ID const* ids, hdf5::Shape const* shapes)
    {
        for (Index o = 0; o < nr_objects; ++o)
        {
            // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
            expand_core(ids[o], shapes[o]);
        }

        _nr_objects += nr_objects;

        attributes().write<Count>(nr_objects_tag, _nr_objects);
    }


    /*!
        @brief      Make space for additional object arrays of objects
        @param      nr_objects Number of objects (equals number of object arrays and number of value arrays)
        @param      ids For each object, the object ID
        @param      shapes For each object, the shape of the object array. For each object, @a shapes must
                    contain rank sizes.
    */
    void Value::expand(Count const nr_objects, ID const* ids, hdf5::Shape::value_type const* shapes)
    {
        Rank const rank{this->rank()};

        for (Index o = 0, s = 0; o < nr_objects; ++o, s += rank)
        {
            // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
            expand_core(ids[o], hdf5::Shape{shapes[s], shapes[s + 1]});
        }

        _nr_objects += nr_objects;

        attributes().write<Count>(nr_objects_tag, _nr_objects);
    }


    /*!
        @brief      Make space for an additional object array
        @param      id The object ID
        @param      shape The shape of the object array
    */
    void Value::expand(ID const id, hdf5::Shape const& shape, void const* no_data_value)
    {
        expand_core(id, shape, no_data_value);

        _nr_objects += 1;

        attributes().write<Count>(nr_objects_tag, _nr_objects);
    }


    /*!
        @brief      Make space for an additional object array
        @param      id The object ID
        @param      shape The shape of the object array
    */
    void Value::expand_core(ID const id, hdf5::Shape const& array_shape, void const* no_data_value)
    {
        assert(!contains(id));

        std::string const name{std::to_string(id)};
        hdf5::Shape const& max_dimension_sizes{array_shape};
        hdf5::Dataspace const dataspace{hdf5::create_dataspace(array_shape, max_dimension_sizes)};

        hdf5::Dataset::CreationPropertyList creation_property_list{};

        // auto chunk_dimension_sizes =
        //     hdf5::chunk_shape(array_shape, file_datatype.size());
        // creation_property_list.set_chunk(chunk_dimension_sizes);

        if (no_data_value)
        {
            creation_property_list.set_fill_value(memory_datatype(), no_data_value);
        }

        /* auto dataset = */ hdf5::create_dataset(
            this->id(), name, file_datatype(), dataspace, creation_property_list);
    }


    auto Value::contains(ID const id) const -> bool
    {
        std::string const name{std::to_string(id)};

        return hdf5::dataset_exists(this->id(), name);
    }


    auto Value::operator[](ID const id) const -> Array
    {
        assert(contains(id));

        std::string const name{std::to_string(id)};

        return Array{*this, name, memory_datatype()};
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

        group.attributes().write<Count>(nr_objects_tag, 0);

        return Value{std::move(group)};
    }

}  // namespace lue::data_model::different_shape
