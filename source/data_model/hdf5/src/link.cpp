#include "lue/hdf5/link.hpp"
#include "lue/hdf5/group.hpp"
#include <fmt/format.h>
#include <cassert>


namespace lue {
namespace hdf5 {

Link::Link(
    Group& group,
    std::string const& name):

    _location_id{group.id()},
    _name{name}

{
}


Identifier const& Link::location_id() const
{
    return _location_id;
}


std::string const& Link::name() const
{
    return _name;
}


::H5L_info_t Link::info() const
{
    return link_info(_location_id, _name);
}


std::size_t Link::value_size() const
{
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-union-access)
    return info().u.val_size;
}


bool link_exists(
    Identifier const& id,
    std::string const& name)
{
    return ::H5Lexists(id, name.c_str(), H5P_DEFAULT) > 0;
}


H5O_type_t object_type(
    Identifier const& id,
    std::string const& name)
{
    ::H5O_info_t info;

    ::herr_t const status{::H5Oget_info_by_name(id, name.c_str(), &info, H5P_DEFAULT)};

    if(status < 0) {
        throw std::runtime_error(fmt::format(
                "Information about object {} at {} cannot be obtained",
                name, id.pathname()
            ));
    }

    return info.type;
}


::H5L_info_t link_info(
    Identifier const& id,
    std::string const& name)
{
    if(!link_exists(id, name)) {
        throw std::runtime_error(fmt::format(
                "Link {} does not exist at {}",
                name, id.pathname()
            ));
    }

    ::H5L_info_t info;
    ::herr_t const status{::H5Lget_info(id, name.c_str(), &info, H5P_DEFAULT)};

    if(status < 0) {
        throw std::runtime_error(fmt::format(
                "Information about link {} at {} cannot be obtained",
                name, id.pathname()
            ));
    }

    return info;
}


::H5L_type_t link_type(
    Identifier const& id,
    std::string const& name)
{
    return link_info(id, name).type;
}


bool link_is_dataset(
    Identifier const& id,
    std::string const& name)
{
    return object_type(id, name) == ::H5O_TYPE_DATASET;
}


bool link_is_group(
    Identifier const& id,
    std::string const& name)
{
    return object_type(id, name) == ::H5O_TYPE_GROUP;
}

}  // namespace hdf5
}  // namespace lue
