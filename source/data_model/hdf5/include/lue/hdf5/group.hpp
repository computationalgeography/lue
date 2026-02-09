#pragma once
#include "lue/hdf5/hard_link.hpp"
#include "lue/hdf5/primary_data_object.hpp"
#include "lue/hdf5/soft_link.hpp"


namespace lue::hdf5 {

    // Forward declare the File class to be able to declare it a friend of the Group class
    class File;


    /*!
        @brief      This class represents an open HDF5 group

        An HDF5 group can be opened multiple times.
    */
    class LUE_HDF5_EXPORT Group: public PrimaryDataObject
    {

        public:

            Group(Group const& parent, std::string const& name);

            Group(Group const& parent, Identifier&& id);

            explicit Group(Identifier&& id);

            auto has_parent() const -> bool;

            auto parent_pathname() const -> std::string const&;

            auto parent() const -> Group;

            auto group_names() const -> std::vector<std::string>;

            auto object_names() const -> std::vector<std::string>;

            auto contains_group(std::string const& name) const -> bool;

            auto contains_dataset(std::string const& name) const -> bool;

            auto contains_soft_link(std::string const& name) const -> bool;

            auto contains_hard_link(std::string const& name) const -> bool;

            auto soft_link(std::string const& name) -> SoftLink;

            auto hard_link(std::string const& name) -> HardLink;

            auto create_soft_link(Identifier const& location, std::string const& name) -> SoftLink;

            auto create_hard_link(Identifier const& location, std::string const& name) -> HardLink;

        private:

            friend File;

            Group(Identifier const& parent, std::string const& name);

            //! Pathname of parent group
            std::string _parent_pathname;
    };


    LUE_HDF5_EXPORT auto group_exists(Group const& parent, std::string const& name) -> bool;

    LUE_HDF5_EXPORT auto create_group(Group& parent, std::string const& name) -> Group;

}  // namespace lue::hdf5
