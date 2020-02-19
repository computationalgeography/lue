#include "lue/info/identity/active_object_id.hpp"
#include "lue/core/tag.hpp"


namespace lue {
namespace data_model {

ActiveObjectID::ActiveObjectID(
    hdf5::Group& parent):

    same_shape::constant_shape::Value{parent, active_object_id_tag}

{
}


ActiveObjectID::ActiveObjectID(
    same_shape::constant_shape::Value&& value):

    same_shape::constant_shape::Value{
        std::forward<same_shape::constant_shape::Value>(value)}

{
}


Count ActiveObjectID::nr_ids() const
{
    return nr_arrays();
}


ActiveObjectID create_active_object_id(
    hdf5::Group& parent)
{
    auto value = same_shape::constant_shape::create_value(
        parent, active_object_id_tag, hdf5::Datatype{H5T_NATIVE_HSIZE});

    return ActiveObjectID{std::move(value)};
}

}  // namespace data_model
}  // namespace lue
