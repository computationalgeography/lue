#include "lue/info/identity/id.hpp"
#include "lue/core/tag.hpp"


namespace lue {
namespace info {

ID::ID(
    hdf5::Group& parent)
:
    same_shape::Value{parent, id_tag}

{
}


ID::ID(
    same_shape::Value&& value)
:
    same_shape::Value{std::forward<same_shape::Value>(value)}

{
}


Count ID::nr_objects() const
{
    return nr_arrays();
}


ID create_id(
    hdf5::Group& parent)
{
    auto value = same_shape::create_value(
        parent, id_tag, hdf5::Datatype{H5T_NATIVE_HSIZE});

    return ID{std::move(value)};
}

}  // namespace info
}  // namespace lue
