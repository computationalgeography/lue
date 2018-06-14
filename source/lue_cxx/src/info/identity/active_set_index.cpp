#include "lue/info/identity/active_set_index.hpp"
#include "lue/core/tag.hpp"


namespace lue {

ActiveSetIndex::ActiveSetIndex(
    hdf5::Group const& parent,
    std::string const& name)
:
    same_shape::constant_shape::Value{parent, name}

{
}


ActiveSetIndex::ActiveSetIndex(
    same_shape::constant_shape::Value&& value)
:
    same_shape::constant_shape::Value{
        std::forward<same_shape::constant_shape::Value>(value)}

{
}


ActiveSetIndex create_active_set_index(
    hdf5::Group& parent)
{
    return ActiveSetIndex{same_shape::constant_shape::create_value(
        parent, active_set_index_tag, hdf5::Datatype{H5T_NATIVE_HSIZE})};
}

}  // namespace lue
