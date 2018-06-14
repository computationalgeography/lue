#include "lue/info/identity/active_object_index.hpp"
#include "lue/core/tag.hpp"


namespace lue {

ActiveObjectIndex::ActiveObjectIndex(
    hdf5::Group const& parent,
    std::string const& name)
:
    same_shape::constant_shape::Value{parent, name}

{
}


ActiveObjectIndex::ActiveObjectIndex(
    same_shape::constant_shape::Value&& value)
:
    same_shape::constant_shape::Value{
        std::forward<same_shape::constant_shape::Value>(value)}

{
}


ActiveObjectIndex create_active_object_index(
    hdf5::Group& parent)
{
    return ActiveObjectIndex{same_shape::constant_shape::create_value(
        parent, active_object_index_tag, hdf5::Datatype{H5T_NATIVE_HSIZE})};
}

}  // namespace lue
