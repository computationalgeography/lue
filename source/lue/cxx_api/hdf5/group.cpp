#include "lue/cxx_api/hdf5/group.h"
#include <cassert>


namespace lue {
namespace hdf5 {

Group::Group(
    Identifier&& id)

    : _id{std::forward<Identifier>(id)},
      _attributes{_id}

{
}


Group::Group(
    Group&& other)

    : _id{std::move(other._id)},
      _attributes{_id}

{
    // Invalidate other.
    std::move(other);
}


Group& Group::operator=(
    Group&& other)
{
    _id = std::move(other._id);
    _attributes = Attributes{_id};

    // Invalidate other.
    std::move(other);

    return *this;
}


Identifier const& Group::id() const
{
    return _id;
}


Attributes const& Group::attributes() const
{
    return _attributes;
}

} // namespace hdf5
} // namespace lue
