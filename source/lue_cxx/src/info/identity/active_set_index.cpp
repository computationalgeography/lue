#include "lue/info/identity/active_set_index.hpp"
#include "lue/core/tag.hpp"


namespace lue {
namespace info {

ActiveSetIndex::ActiveSetIndex(
    hdf5::Group const& parent)
:
    same_shape::constant_shape::Value{parent, active_set_index_tag}

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
    auto value = same_shape::constant_shape::create_value(
        parent, active_set_index_tag, hdf5::Datatype{H5T_NATIVE_HSIZE});

    return ActiveSetIndex{std::move(value)};
}

}  // namespace info
}  // namespace lue
