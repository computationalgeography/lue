#include "lue/hdf5/group.hpp"
#include "lue/hdf5/dataset.hpp"
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <format>


namespace lue::hdf5 {
    namespace {

        template<typename F, typename T>
        void iterate(Identifier const& location, F& callback, T& data)
        {
            herr_t const status{
                H5Literate(location, H5_INDEX_NAME, H5_ITER_NATIVE, nullptr, callback, &data)};

            if (status < 0)
            {
                throw std::runtime_error(
                    std::format("Cannot iterate through links in group at {}", location.pathname()));
            }
        }


        class ObjectNameCollector
        {

            public:

                using Names = std::vector<std::string>;

                void add(std::string name)
                {
                    _names.emplace_back(std::move(name));
                }

                auto names() && -> Names&&
                {
                    return std::move(_names);
                }

            private:

                Names _names;
        };


        auto retrieve_object_names(
            hid_t const location, char const* name, [[maybe_unused]] H5L_info_t const* info, void* data)
            -> herr_t
        {
            H5O_info_t infobuf;
            herr_t const status{H5Oget_info_by_name(location, name, &infobuf, H5P_DEFAULT)};

            if (status < 0)
            {
                return status;
            }

            if (infobuf.type == H5O_TYPE_GROUP || infobuf.type == H5O_TYPE_DATASET)
            {
                static_cast<ObjectNameCollector*>(data)->add(name);
            }

            return 0;
        }


        auto retrieve_group_names(
            hid_t const location, char const* name, [[maybe_unused]] H5L_info_t const* info, void* data)
            -> herr_t
        {
            H5O_info_t infobuf;
            herr_t const status{H5Oget_info_by_name(location, name, &infobuf, H5P_DEFAULT)};

            if (status < 0)
            {
                return status;
            }

            if (infobuf.type == H5O_TYPE_GROUP)
            {
                static_cast<ObjectNameCollector*>(data)->add(name);
            }

            return 0;
        }

    }  // Anonymous namespace


    Group::Group(Identifier const& parent, std::string const& name):

        PrimaryDataObject{Identifier{H5Gopen(parent, name.c_str(), H5P_DEFAULT), H5Gclose}},
        _parent_pathname{}

    {
        if (!id().is_valid())
        {
            throw std::runtime_error(std::format("Cannot open group {} in {}", name, parent.pathname()));
        }

        std::filesystem::path const path{id().pathname()};
        assert(path.is_absolute());

        if (path.has_parent_path())
        {
            _parent_pathname = path.parent_path().string();
        }

        assert(id().is_valid());
        assert(has_parent() || _parent_pathname.empty());
    }


    /*!
        @brief      Open group @a name in @a parent
        @param      parent Parent group of group to open
        @param      name Name of group to open
        @exception  std::runtime_error In case the group cannot be opened
    */
    Group::Group(Group const& parent, std::string const& name):

        Group{parent.id(), name}

    {
    }


    Group::Group(Group const& parent, Identifier&& id):

        PrimaryDataObject{std::move(id)},
        _parent_pathname{parent.id().pathname()}

    {
        // Only used by create_group() below
        assert(this->id().is_valid());
        assert(has_parent());
    }


    /*!
        @brief      Construct a group based on an identifier
        @param      id Identifier of group
        @warning    Whether or not the @a id passed in is valid must be checked by the calling code

        Use this constructor for groups that have no parent.
    */
    Group::Group(Identifier&& id):

        PrimaryDataObject{std::move(id)},
        _parent_pathname{}

    {
        // Only used by File constructor

        // No assertions please. It is well possible that the id is not valid. The calling code must check
        // that.

        // assert(this->id().is_valid());
        // assert(this->id().type() == H5I_FILE);
    }


    auto Group::has_parent() const -> bool
    {
        return !_parent_pathname.empty();
    }


    auto Group::parent() const -> Group
    {
        assert(has_parent());

        return Group{id().file_id(), _parent_pathname};
    }


    auto Group::parent_pathname() const -> std::string const&
    {
        return _parent_pathname;
    }


    /*!
        @brief      Return the names of the child-groups
    */
    auto Group::group_names() const -> std::vector<std::string>
    {
        ObjectNameCollector data{};
        iterate(id(), retrieve_group_names, data);

        return std::move(data).names();
    }


    /*!
        @brief      Return the names of the child-objects
    */
    auto Group::object_names() const -> std::vector<std::string>
    {
        ObjectNameCollector data{};
        iterate(id(), retrieve_object_names, data);

        return std::move(data).names();
    }


    /*!
        @brief      Return whether or not the group contains a child-group named @a name
    */
    auto Group::contains_group(std::string const& name) const -> bool
    {
        return group_exists(*this, name);
    }


    /*!
        @brief      Return whether or not the group contains a dataset named @a name
    */
    auto Group::contains_dataset(std::string const& name) const -> bool
    {
        return dataset_exists(id(), name);
    }


    /*!
        @brief      Return whether or not the group contains a soft-link named @a name
    */
    auto Group::contains_soft_link(std::string const& name) const -> bool
    {
        return soft_link_exists(*this, name);
    }


    /*!
        @brief      Return whether or not the group contains a hard-link named @a name
    */
    auto Group::contains_hard_link(std::string const& name) const -> bool
    {
        return hard_link_exists(*this, name);
    }


    auto Group::soft_link(std::string const& name) -> SoftLink
    {
        assert(contains_soft_link(name));

        return SoftLink{*this, name};
    }


    auto Group::hard_link(std::string const& name) -> HardLink
    {
        assert(contains_hard_link(name));

        return HardLink{*this, name};
    }


    /*!
        @brief      Add a soft-link named @a name to @a location to the group
        @param      location Identifier to object to link to
        @param      name Name of soft-link to create
    */
    auto Group::create_soft_link(Identifier const& location, std::string const& name) -> SoftLink
    {
        return hdf5::create_soft_link(*this, location, name);
    }


    /*!
        @brief      Add a hard-link named @a name to @a location to the group
        @param      location Identifier to object to link to
        @param      name Name of hard-link to create
    */
    auto Group::create_hard_link(Identifier const& location, std::string const& name) -> HardLink
    {
        return hdf5::create_hard_link(*this, location, name);
    }


    auto group_exists(Group const& parent, std::string const& name) -> bool
    {
        return link_exists(parent.id(), name) && link_is_group(parent.id(), name);
    }


    /*!
        @brief      Create a group named @a name within @a parent
        @param      parent Group to create group in
        @param      name Name of group to create
    */
    auto create_group(Group& parent, std::string const& name) -> Group
    {
        if (group_exists(parent, name))
        {
            throw std::runtime_error(std::format("Group {} already exists", name));
        }

        Identifier group_id{
            H5Gcreate(parent.id(), name.c_str(), H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT), H5Gclose};

        if (!group_id.is_valid())
        {
            throw std::runtime_error(
                std::format("Cannot create group {} at {}", name, parent.id().pathname()));
        }

        return {parent, std::move(group_id)};
    }

}  // namespace lue::hdf5
