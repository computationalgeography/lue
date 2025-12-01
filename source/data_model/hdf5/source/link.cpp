#include "lue/hdf5/link.hpp"
#include "lue/hdf5/group.hpp"
#include <cassert>
#include <format>
#include <utility>


namespace lue::hdf5 {

    Link::Link(Group const& group, std::string name):

        _location_id{group.id()},
        _name{std::move(name)}

    {
    }


    auto Link::location_id() const -> Identifier const&
    {
        return _location_id;
    }


    auto Link::name() const -> std::string const&
    {
        return _name;
    }


    auto Link::info() const -> H5L_info_t
    {
        return link_info(_location_id, _name);
    }


    auto Link::value_size() const -> std::size_t
    {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-union-access)
        return info().u.val_size;
    }


    auto link_exists(Identifier const& id, std::string const& name) -> bool
    {
        return H5Lexists(id, name.c_str(), H5P_DEFAULT) > 0;
    }


    auto object_type(Identifier const& id, std::string const& name) -> H5O_type_t
    {
        H5O_info_t info;

        herr_t const status{H5Oget_info_by_name(id, name.c_str(), &info, H5P_DEFAULT)};

        if (status < 0)
        {
            throw std::runtime_error(
                std::format("Information about object {} at {} cannot be obtained", name, id.pathname()));
        }

        return info.type;
    }


    auto link_info(Identifier const& id, std::string const& name) -> H5L_info_t
    {
        if (!link_exists(id, name))
        {
            throw std::runtime_error(std::format("Link {} does not exist at {}", name, id.pathname()));
        }

        H5L_info_t info;
        herr_t const status{H5Lget_info(id, name.c_str(), &info, H5P_DEFAULT)};

        if (status < 0)
        {
            throw std::runtime_error(
                std::format("Information about link {} at {} cannot be obtained", name, id.pathname()));
        }

        return info;
    }


    auto link_type(Identifier const& id, std::string const& name) -> H5L_type_t
    {
        return link_info(id, name).type;
    }


    auto link_is_dataset(Identifier const& id, std::string const& name) -> bool
    {
        return object_type(id, name) == H5O_TYPE_DATASET;
    }


    auto link_is_group(Identifier const& id, std::string const& name) -> bool
    {
        return object_type(id, name) == H5O_TYPE_GROUP;
    }

}  // namespace lue::hdf5
