#include "lue/info/identity/active_id.hpp"
#include "lue/core/tag.hpp"


namespace lue {

ActiveID::ActiveID(
    hdf5::Group const& parent,
    std::string const& name)
:
    same_shape::constant_shape::Value{parent, name}

{
}


ActiveID::ActiveID(
    same_shape::constant_shape::Value&& value)
:
    same_shape::constant_shape::Value{
        std::forward<same_shape::constant_shape::Value>(value)}

{
}


ActiveID create_active_id(
    hdf5::Group& parent)
{
    return ActiveID{same_shape::constant_shape::create_value(
        parent, active_id_tag, hdf5::Datatype{H5T_NATIVE_HSIZE})};
}

}  // namespace lue
