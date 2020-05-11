#include "lue/hdf5/group.hpp"
#include "lue/hdf5/dataset.hpp"
#include <fmt/format.h>
#include <boost/filesystem.hpp>
#include <cstring>
#include <cstdlib>
#include <memory>
#include <iostream>


namespace lue {
namespace hdf5 {
namespace {

template<
    typename F,
    typename T>
void iterate(
    Identifier const& location,
    F& callback,
    T& data)
{
    auto const status = ::H5Literate(
        location, ::H5_INDEX_NAME, ::H5_ITER_NATIVE, nullptr, callback, &data);

    if(status < 0) {
        throw std::runtime_error(fmt::format(
            "Cannot iterate through links in group at {}",
            location.pathname()
        ));
    }
}


// class ObjectCounter
// {
// 
//     public:
// 
//         ObjectCounter():
//             _nr_objects{0}
//         {
//         }
// 
//         std::size_t operator++()
//         {
//             return ++_nr_objects;
//         }
// 
//         std::size_t nr_objects() const
//         {
//             return _nr_objects;
//         }
// 
//     private:
// 
//         std::size_t _nr_objects;
// 
// };


// herr_t count_objects(
//     hid_t const location,
//     char const* name,
//     ::H5L_info_t const* /* info */,
//     void* data)
// {
//     auto* data_ = static_cast<ObjectCounter*>(data);
// 
//     ::H5O_info_t infobuf;
//     auto status = ::H5Oget_info_by_name(location, name, &infobuf, H5P_DEFAULT);
// 
//     if(status < 0) {
//         return status;
//     }
// 
//     if(infobuf.type == H5O_TYPE_GROUP || infobuf.type == H5O_TYPE_DATASET) {
//         ++data_;
//     }
// 
//     return 0;
// }


// herr_t count_groups(
//     hid_t const location,
//     char const* name,
//     ::H5L_info_t const* /* info */,
//     void* data)
// {
//     auto* data_ = static_cast<ObjectCounter*>(data);
// 
//     ::H5O_info_t infobuf;
//     auto status = ::H5Oget_info_by_name(location, name, &infobuf, H5P_DEFAULT);
// 
//     if(status < 0) {
//         return status;
//     }
// 
//     if(infobuf.type == H5O_TYPE_GROUP) {
//         ++data_;
//     }
// 
//     return 0;
// }


// herr_t count_datasets(
//     hid_t const location,
//     char const* name,
//     ::H5L_info_t const* /* info */,
//     void* data)
// {
//     auto* data_ = static_cast<ObjectCounter*>(data);
// 
//     ::H5O_info_t infobuf;
//     auto status = ::H5Oget_info_by_name(location, name, &infobuf, H5P_DEFAULT);
// 
//     if(status < 0) {
//         return status;
//     }
// 
//     if(infobuf.type == H5O_TYPE_DATASET) {
//         ++data_->nr_objects;
//     }
// 
//     return 0;
// }


class ObjectNameCollector
{

    public:

        using Names = std::vector<std::string>;

        void add(
            std::string name)
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


herr_t retrieve_object_names(
    ::hid_t const location,
    char const* name,
    ::H5L_info_t const* /* info */,
    void* data)
{
    ::H5O_info_t infobuf;
    herr_t status = H5Oget_info_by_name(location, name, &infobuf, H5P_DEFAULT);

    if(status < 0) {
        return status;
    }

    if(infobuf.type == H5O_TYPE_GROUP || infobuf.type == H5O_TYPE_DATASET)
    {
        auto* data_ = static_cast<ObjectNameCollector*>(data);
        data_->add(name);
    }

    return 0;
}


herr_t retrieve_group_names(
    ::hid_t const location,
    char const* name,
    ::H5L_info_t const* /* info */,
    void* data)
{
    ::H5O_info_t infobuf;
    herr_t status = H5Oget_info_by_name(location, name, &infobuf, H5P_DEFAULT);

    if(status < 0) {
        return status;
    }

    if(infobuf.type == H5O_TYPE_GROUP) {
        auto* data_ = static_cast<ObjectNameCollector*>(data);
        data_->add(name);
    }

    return 0;
}


// herr_t retrieve_dataset_names(
//     ::hid_t const location,
//     char const* name,
//     ::H5L_info_t const* /* info */,
//     void* data)
// {
//     auto* data_ = static_cast<ObjectNameCollector*>(data);
// 
//     ::H5O_info_t infobuf;
//     herr_t status = H5Oget_info_by_name(location, name, &infobuf, H5P_DEFAULT);
// 
//     if(status < 0) {
//         return status;
//     }
// 
//     if(infobuf.type == H5O_TYPE_DATASET) {
//         data_->_names[data_->_nr_names] =
//             (char*)malloc((std::strlen(name) + 1) * sizeof(char));
//         std::strcpy(data_->_names[data_->_nr_names], name);
//         ++data_->_nr_names;
//     }
// 
//     return 0;
// }

}  // Anonymous namespace


/*!
    @brief      Open group @a name in @a parent
    @param      parent Parent group of group to open
    @param      name Name of group to open
    @exception  std::runtime_error In case the group cannot be opened
*/
Group::Group(
    Group& parent,
    std::string const& name):

    PrimaryDataObject{
            Identifier{
                ::H5Gopen(parent.id(), name.c_str(), H5P_DEFAULT),
                ::H5Gclose
            }
        },
    _parent{}

{
    if(!id().is_valid()) {
        throw std::runtime_error(fmt::format(
            "Cannot open group {} in {}",
            name, parent.id().pathname()
        ));
    }

    boost::filesystem::path path{id().pathname()};
    assert(path.is_absolute());

    if(parent.id().pathname() == path.parent_path().string()) {
        // Parent passed in is our direct parent. Cool, use it.
        _parent = std::make_unique<Group>(parent);
    }
    else if(path.has_parent_path()) {
        // Direct parent is different from parent passed in. This happens
        // when using absolute paths as group names and pass in an
        // unrelated parent, which is OK. Here we build up the chain of
        // direct parents.
        _parent = std::make_unique<Group>(*this, path.parent_path().string());
    }
    // else {
    //   Parent is root
    // }

    assert(id().is_valid());
}


Group::Group(
    Group& parent,
    Identifier& id):

    PrimaryDataObject{id},
    _parent{std::make_unique<Group>(parent)}

{
    assert(this->id().is_valid());
}


Group::Group(
    Group& other):

    PrimaryDataObject{other},
    _parent{}

{
    if(other._parent) {
        _parent = std::make_unique<Group>(*other._parent);
    }
}


/*!
    @brief      Construct a group based on an identifier
    @param      id Identifier of group

    Use this constructor for groups that have no parent.
*/
Group::Group(
    Identifier&& id):

    PrimaryDataObject{std::forward<Identifier>(id)},
    _parent{}

{
    // assert(id().is_valid());
}


bool Group::has_parent() const
{
    return bool{_parent};
}


Group& Group::parent()
{
    assert(_parent);

    return *_parent;
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
bool Group::contains_group(
    std::string const& name) const
{
    return group_exists(*this, name);
}


/*!
    @brief      Return whether or not the group contains a dataset named
                @a name
*/
bool Group::contains_dataset(
    std::string const& name) const
{
    return dataset_exists(id(), name);
}


/*!
    @brief      Return whether or not the group contains a soft-link named
                @a name
*/
bool Group::contains_soft_link(
    std::string const& name) const
{
    return soft_link_exists(*this, name);
}


/*!
    @brief      Return whether or not the group contains a hard-link named
                @a name
*/
bool Group::contains_hard_link(
    std::string const& name) const
{
    return hard_link_exists(*this, name);
}


SoftLink Group::soft_link(
    std::string const& name)
{
    assert(contains_soft_link(name));

    return SoftLink{*this, name};
}


HardLink Group::hard_link(
    std::string const& name)
{
    assert(contains_hard_link(name));

    return HardLink{*this, name};
}


/*!
    @brief      Add a soft-link named @a name to @a location to the group
    @param      location Identifier to object to link to
    @param      name Name of soft-link to create
*/
SoftLink Group::create_soft_link(
    Identifier const& location,
    std::string const& name)
{
    return hdf5::create_soft_link(*this, location, name);
}


/*!
    @brief      Add a hard-link named @a name to @a location to the group
    @param      location Identifier to object to link to
    @param      name Name of hard-link to create
*/
HardLink Group::create_hard_link(
    Identifier const& location,
    std::string const& name)
{
    return hdf5::create_hard_link(*this, location, name);
}


bool group_exists(
    Group const& parent,
    std::string const& name)
{
    return
        link_exists(parent.id(), name) &&
        link_is_group(parent.id(), name)
        ;
}


/*!
    @brief      Create a group named @a name within @a parent
    @param      parent Group to create group in
    @param      name Name of group to create
*/
Group create_group(
    Group& parent,
    std::string const& name)
{
    if(group_exists(parent, name)) {
        throw std::runtime_error(fmt::format(
            "Group {} already exists",
            name
        ));
    }

    Identifier group_id{
            ::H5Gcreate(parent.id(), name.c_str(), H5P_DEFAULT, H5P_DEFAULT,
                H5P_DEFAULT),
            ::H5Gclose
        };

    if(!group_id.is_valid()) {
        throw std::runtime_error(fmt::format(
            "Cannot create group {} at {}",
            name, parent.id().pathname()
        ));
    }

    return Group{parent, group_id};
}

} // namespace hdf5
} // namespace lue
