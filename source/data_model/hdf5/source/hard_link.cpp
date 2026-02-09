#include "lue/hdf5/hard_link.hpp"
#include "lue/hdf5/group.hpp"
#include <format>


namespace lue::hdf5 {

    HardLink::HardLink(Group const& group, std::string const& name):

        Link{group, name}

    {
        assert(hard_link_exists(group, name));
    }


    auto hard_link_exists(Group const& group, std::string const& name) -> bool
    {
        return link_exists(group.id(), name) && link_type(group.id(), name) == H5L_TYPE_HARD;
    }


    auto create_hard_link(Group& group, Identifier const& target, std::string const& name) -> HardLink
    {
        herr_t const status{H5Lcreate_hard(
            target, target.pathname().c_str(), group.id(), name.c_str(), H5P_DEFAULT, H5P_DEFAULT)};

        if (status < 0)
        {
            throw std::runtime_error(
                std::format("Cannot create hard link {} at {}", name, target.pathname()));
        }

        return HardLink{group, name};
    }

}  // namespace lue::hdf5
