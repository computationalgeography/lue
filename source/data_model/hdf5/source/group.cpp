#include "lue/hdf5/group.hpp"
#include "lue/hdf5/dataset.hpp"
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <format>


namespace lue {
    namespace hdf5 {
        namespace {

            template<typename F, typename T>
            void iterate(Identifier const& location, F& callback, T& data)
            {
                ::herr_t const status{
                    ::H5Literate(location, ::H5_INDEX_NAME, ::H5_ITER_NATIVE, nullptr, callback, &data)};

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

                    Names&& names() &&
                    {
                        return std::move(_names);
                    }

                private:

                    Names _names;
            };


            ::herr_t retrieve_object_names(
                ::hid_t const location, char const* name, ::H5L_info_t const* /* info */, void* data)
            {
                ::H5O_info_t infobuf;
                ::herr_t const status{::H5Oget_info_by_name(location, name, &infobuf, H5P_DEFAULT)};

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


            ::herr_t retrieve_group_names(
                ::hid_t const location, char const* name, ::H5L_info_t const* /* info */, void* data)
            {
                ::H5O_info_t infobuf;
                ::herr_t const status{::H5Oget_info_by_name(location, name, &infobuf, H5P_DEFAULT)};

                if (status < 0)
                {
                    return status;
                }

                if (infobuf.type == ::H5O_TYPE_GROUP)
                {
                    static_cast<ObjectNameCollector*>(data)->add(name);
                }

                return 0;
            }

        }  // Anonymous namespace


        Group::Group(Identifier const& parent, std::string const& name):

            PrimaryDataObject{Identifier{::H5Gopen(parent, name.c_str(), H5P_DEFAULT), ::H5Gclose}},
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
            @warning    Whether or not the @a id passed in is valid must be
                        checked by the calling code

            Use this constructor for groups that have no parent.
        */
        Group::Group(Identifier&& id):

            PrimaryDataObject{std::move(id)},
            _parent_pathname{}

        {
            // Only used by File constructor

            // No assertions please. It is well possible that the id is not
            // valid. The calling code must check that.

            // assert(this->id().is_valid());
            // assert(this->id().type() == ::H5I_FILE);
        }


        bool Group::has_parent() const
        {
            return !_parent_pathname.empty();
        }


        Group Group::parent() const
        {
            assert(has_parent());

            return Group{id().file_id(), _parent_pathname};
        }


        std::string const& Group::parent_pathname() const
        {
            return _parent_pathname;
        }


        /*!
            @brief      Return the names of the child-groups
        */
        std::vector<std::string> Group::group_names() const
        {
            ObjectNameCollector data{};
            iterate(id(), retrieve_group_names, data);

            return std::move(data).names();
        }


        /*!
            @brief      Return the names of the child-objects
        */
        std::vector<std::string> Group::object_names() const
        {
            ObjectNameCollector data{};
            iterate(id(), retrieve_object_names, data);

            return std::move(data).names();
        }


        /*!
            @brief      Return whether or not the group contains a child-group named
                        @a name
        */
        bool Group::contains_group(std::string const& name) const
        {
            return group_exists(*this, name);
        }


        /*!
            @brief      Return whether or not the group contains a dataset named
                        @a name
        */
        bool Group::contains_dataset(std::string const& name) const
        {
            return dataset_exists(id(), name);
        }


        /*!
            @brief      Return whether or not the group contains a soft-link named
                        @a name
        */
        bool Group::contains_soft_link(std::string const& name) const
        {
            return soft_link_exists(*this, name);
        }


        /*!
            @brief      Return whether or not the group contains a hard-link named
                        @a name
        */
        bool Group::contains_hard_link(std::string const& name) const
        {
            return hard_link_exists(*this, name);
        }


        SoftLink Group::soft_link(std::string const& name)
        {
            assert(contains_soft_link(name));

            return SoftLink{*this, name};
        }


        HardLink Group::hard_link(std::string const& name)
        {
            assert(contains_hard_link(name));

            return HardLink{*this, name};
        }


        /*!
            @brief      Add a soft-link named @a name to @a location to the group
            @param      location Identifier to object to link to
            @param      name Name of soft-link to create
        */
        SoftLink Group::create_soft_link(Identifier const& location, std::string const& name)
        {
            return hdf5::create_soft_link(*this, location, name);
        }


        /*!
            @brief      Add a hard-link named @a name to @a location to the group
            @param      location Identifier to object to link to
            @param      name Name of hard-link to create
        */
        HardLink Group::create_hard_link(Identifier const& location, std::string const& name)
        {
            return hdf5::create_hard_link(*this, location, name);
        }


        bool group_exists(Group const& parent, std::string const& name)
        {
            return link_exists(parent.id(), name) && link_is_group(parent.id(), name);
        }


        /*!
            @brief      Create a group named @a name within @a parent
            @param      parent Group to create group in
            @param      name Name of group to create
        */
        Group create_group(Group& parent, std::string const& name)
        {
            if (group_exists(parent, name))
            {
                throw std::runtime_error(std::format("Group {} already exists", name));
            }

            Identifier group_id{
                ::H5Gcreate(parent.id(), name.c_str(), H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT), ::H5Gclose};

            if (!group_id.is_valid())
            {
                throw std::runtime_error(
                    std::format("Cannot create group {} at {}", name, parent.id().pathname()));
            }

            return Group{parent, std::move(group_id)};
        }

    }  // namespace hdf5
}  // namespace lue
