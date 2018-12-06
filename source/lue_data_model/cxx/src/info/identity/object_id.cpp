#include "lue/info/identity/object_id.hpp"
#include "lue/core/tag.hpp"


namespace lue {

ObjectID::ObjectID(
    hdf5::Group& parent):

    same_shape::Value{parent, object_id_tag}

{
}


ObjectID::ObjectID(
    same_shape::Value&& value)
:
    same_shape::Value{std::forward<same_shape::Value>(value)}

{
}


Count ObjectID::nr_objects() const
{
    return nr_arrays();
}


ObjectID create_object_id(
    hdf5::Group& parent)
{
    auto value = same_shape::create_value(
        parent, object_id_tag, hdf5::Datatype{H5T_NATIVE_HSIZE});

    return ObjectID{std::move(value)};
}

}  // namespace lue
