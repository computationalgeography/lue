#pragma once
#include "lue/hdf5/link.hpp"


namespace lue::hdf5 {

    class LUE_HDF5_EXPORT HardLink: public Link
    {

        public:

            HardLink(Group const& group, std::string const& name);

            HardLink(HardLink const& other) = default;

            HardLink(HardLink&& other) = default;

            ~HardLink() = default;

            auto operator=(HardLink const& other) -> HardLink& = default;

            auto operator=(HardLink&& other) -> HardLink& = default;
    };


    auto hard_link_exists(Group const& group, std::string const& name) -> bool;

    auto create_hard_link(Group& group, Identifier const& target, std::string const& name) -> HardLink;

}  // namespace lue::hdf5
