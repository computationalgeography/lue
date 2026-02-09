#include "lue/info/identity/object_id.hpp"
#include "lue/core/tag.hpp"


namespace lue::data_model {

    ObjectID::ObjectID(hdf5::Group const& parent):

        same_shape::Value{parent, object_id_tag}

    {
    }


    ObjectID::ObjectID(same_shape::Value&& value):

        same_shape::Value{std::move(value)}

    {
    }


    auto ObjectID::nr_objects() const -> Count
    {
        return nr_arrays();
    }


    auto create_object_id(hdf5::Group& parent) -> ObjectID
    {
        same_shape::Value value{
            same_shape::create_value(parent, object_id_tag, hdf5::Datatype{H5T_NATIVE_HSIZE})};

        return ObjectID{std::move(value)};
    }

}  // namespace lue::data_model
