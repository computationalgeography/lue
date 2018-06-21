#include "lue/info/identity/active_id.hpp"
#include "lue/core/tag.hpp"


namespace lue {

ActiveID::ActiveID(
    hdf5::Group const& parent)
:
    same_shape::constant_shape::Value{parent, active_id_tag}

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
    auto value = same_shape::constant_shape::create_value(
        parent, active_id_tag, hdf5::Datatype{H5T_NATIVE_HSIZE});

    return ActiveID{std::move(value)};
}

}  // namespace lue
