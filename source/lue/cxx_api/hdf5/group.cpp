#include "lue/cxx_api/hdf5/group.h"
#include "lue/c_api/hdf5/hdf5_group.h"
#include <cassert>


namespace lue {
namespace hdf5 {

/*!
    @ingroup    lue_cxx_api_group
*/
bool group_exists(
    hdf5::Identifier const& location,
    std::string const& name)
{
    return ::hdf5_group_exists(location, name.c_str()) > 0;
}


Group::Group(
    Identifier const& location,
    std::string const& name)

    : _id(::hdf5_open_group(location, name.c_str()), ::hdf5_close_group),
      _attributes{_id}

{
    if(!_id.is_valid()) {
        throw std::runtime_error("Group " + name + " cannot be opened");
    }
}


Group::Group(
    Identifier&& id)

    : _id{std::forward<Identifier>(id)},
      _attributes{_id}

{
    assert(_id.is_valid());
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


std::string Group::name() const
{
    return _id.name();
}


Attributes const& Group::attributes() const
{
    return _attributes;
}


Attributes& Group::attributes()
{
    return _attributes;
}


/*!
    @ingroup    lue_cxx_api_group
*/
Group create_group(
    hdf5::Identifier const& location,
    std::string const& name)
{
    if(group_exists(location, name)) {
        throw std::runtime_error("Group " + name + " already exists");
    }

    hdf5::Identifier group_location(::hdf5_create_group(location, name.c_str()),
        ::hdf5_close_group);

    if(!group_location.is_valid()) {
        throw std::runtime_error("Group " + name + " cannot be created");
    }

    return Group(std::move(group_location));
}

} // namespace hdf5
} // namespace lue
