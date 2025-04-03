#pragma once
#include "lue/hdf5/hard_link.hpp"
#include "lue/hdf5/primary_data_object.hpp"
#include "lue/hdf5/soft_link.hpp"


namespace lue::hdf5 {

    // Forward declare the File class to be able to declare it a friend of
    // the Group class
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

            [[nodiscard]] auto has_parent() const -> bool;

            [[nodiscard]] auto parent_pathname() const -> std::string const&;

            [[nodiscard]] auto parent() const -> Group;

            [[nodiscard]] auto group_names() const -> std::vector<std::string>;

            [[nodiscard]] auto object_names() const -> std::vector<std::string>;

            [[nodiscard]] auto contains_group(std::string const& name) const -> bool;

            [[nodiscard]] auto contains_dataset(std::string const& name) const -> bool;

            [[nodiscard]] auto contains_soft_link(std::string const& name) const -> bool;

            [[nodiscard]] auto contains_hard_link(std::string const& name) const -> bool;

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


    LUE_HDF5_EXPORT bool group_exists(Group const& parent, std::string const& name);

    LUE_HDF5_EXPORT Group create_group(Group& parent, std::string const& name);

}  // namespace lue::hdf5
