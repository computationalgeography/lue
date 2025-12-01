#pragma once
#include "lue/hdf5/link.hpp"


namespace lue::hdf5 {

    class LUE_HDF5_EXPORT SoftLink: public Link
    {

        public:

            SoftLink(Group const& group, std::string const& name);

            SoftLink(SoftLink const& other) = default;

            SoftLink(SoftLink&& other) = default;

            ~SoftLink() = default;

            auto operator=(SoftLink const& other) -> SoftLink& = default;

            auto operator=(SoftLink&& other) -> SoftLink& = default;

            auto path() const -> std::string;
    };


    auto soft_link_exists(Group const& group, std::string const& name) -> bool;

    auto create_soft_link(Group& group, Identifier const& target, std::string const& name) -> SoftLink;

}  // namespace lue::hdf5
