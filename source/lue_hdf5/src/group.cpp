#include "lue/hdf5/group.hpp"
#include "lue/hdf5/dataset.hpp"
#include "lue/hdf5/link.hpp"
#include <fmt/format.h>
#include <cstring>
#include <cstdlib>
#include <memory>


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
    auto const status = ::H5Literate(location, ::H5_INDEX_NAME,
        ::H5_ITER_NATIVE, nullptr, callback, &data);

    if(status < 0) {
        throw std::runtime_error(fmt::format(
            "Cannot iterate through links in group at {}",
            location.pathname()
        ));
    }
}


struct CountObjects
{
    std::size_t nr_objects;

    CountObjects() : nr_objects(0) { }
};


herr_t count_objects(
    hid_t const location,
    char const* name,
    ::H5L_info_t const* /* info */,
    void* data)
{
    CountObjects* data_ = static_cast<CountObjects*>(data);

    ::H5O_info_t infobuf;
    auto status = ::H5Oget_info_by_name(location, name, &infobuf, H5P_DEFAULT);

    if(status < 0) {
        return status;
    }

    if(infobuf.type == H5O_TYPE_GROUP || infobuf.type == H5O_TYPE_DATASET) {
        ++data_->nr_objects;
    }

    return 0;
}


herr_t count_groups(
    hid_t const location,
    char const* name,
    ::H5L_info_t const* /* info */,
    void* data)
{
    CountObjects* data_ = static_cast<CountObjects*>(data);

    ::H5O_info_t infobuf;
    auto status = ::H5Oget_info_by_name(location, name, &infobuf, H5P_DEFAULT);

    if(status < 0) {
        return status;
    }

    if(infobuf.type == H5O_TYPE_GROUP) {
        ++data_->nr_objects;
    }

    return 0;
}


// herr_t count_datasets(
//     hid_t const location,
//     char const* name,
//     ::H5L_info_t const* /* info */,
//     void* data)
// {
//     CountObjects* data_ = static_cast<CountObjects*>(data);
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


struct ObjectNames
{
    std::size_t _nr_names;
    char** _names;

    ObjectNames(char** const names)
        : _nr_names{0},
          _names{names}
    {
    }
};


herr_t retrieve_object_names(
    ::hid_t const location,
    char const* name,
    ::H5L_info_t const* /* info */,
    void* data)
{
    ObjectNames* data_ = static_cast<ObjectNames*>(data);

    ::H5O_info_t infobuf;
    herr_t status = H5Oget_info_by_name(location, name, &infobuf, H5P_DEFAULT);

    if(status < 0) {
        return status;
    }

    if(infobuf.type == H5O_TYPE_GROUP || infobuf.type == H5O_TYPE_DATASET) {
        data_->_names[data_->_nr_names] =
            static_cast<char*>(malloc((std::strlen(name) + 1) * sizeof(char)));
        std::strcpy(data_->_names[data_->_nr_names], name);
        ++data_->_nr_names;
    }

    return 0;
}


herr_t retrieve_group_names(
    ::hid_t const location,
    char const* name,
    ::H5L_info_t const* /* info */,
    void* data)
{
    ObjectNames* data_ = static_cast<ObjectNames*>(data);

    ::H5O_info_t infobuf;
    herr_t status = H5Oget_info_by_name(location, name, &infobuf, H5P_DEFAULT);

    if(status < 0) {
        return status;
    }

    if(infobuf.type == H5O_TYPE_GROUP) {
        data_->_names[data_->_nr_names] =
            static_cast<char*>(malloc((std::strlen(name) + 1) * sizeof(char)));
        std::strcpy(data_->_names[data_->_nr_names], name);
        ++data_->_nr_names;
    }

    return 0;
}


// herr_t retrieve_dataset_names(
//     ::hid_t const location,
//     char const* name,
//     ::H5L_info_t const* /* info */,
//     void* data)
// {
//     ObjectNames* data_ = static_cast<ObjectNames*>(data);
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
    _parent{std::make_unique<Group>(parent)}

{
    if(!id().is_valid()) {
        throw std::runtime_error(fmt::format(
            "Cannot open group {} in {}",
            name, parent.id().pathname()
        ));
    }

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


// Group& Group::operator=(
//     Group& other)
// 
// {
//     PrimaryDataObject::operator=(other);
// 
//     if(other._parent) {
//         _parent = std::make_unique<Group>(*other._parent);
//     }
//     else {
//         _parent.reset();
//     }
// 
//     return *this;
// }


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
    return _parent ? true : false;
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
    std::size_t nr_objects = 0;
    {
        CountObjects data;
        iterate(id(), count_groups, data);
        nr_objects = data.nr_objects;
    }

    auto names = std::make_unique<char*[]>(nr_objects);
    {
        ObjectNames data(names.get());
        iterate(id(), retrieve_group_names, data);
    }

    std::vector<std::string> result(nr_objects);

    for(std::size_t o = 0; o < nr_objects; ++o) {
        result[o] = names[o];
        std::free(names[o]);
    }

    return result;
}


/*!
    @brief      Return the names of the child-objects
*/
std::vector<std::string> Group::object_names() const
{
    std::size_t nr_objects = 0;
    {
        CountObjects data;
        iterate(id(), count_objects, data);
        nr_objects = data.nr_objects;
    }

    auto names = std::make_unique<char*[]>(nr_objects);
    {
        ObjectNames data(names.get());
        iterate(id(), retrieve_object_names, data);
    }

    std::vector<std::string> result(nr_objects);

    for(std::size_t o = 0; o < nr_objects; ++o) {
        result[o] = names[o];
        std::free(names[o]);
    }

    return result;
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
    return soft_link_exists(id(), name);
}


/*!
    @brief      Return whether or not the group contains a hard-link named
                @a name
*/
bool Group::contains_hard_link(
    std::string const& name) const
{
    return hard_link_exists(id(), name);
}


/*!
    @brief      Add a soft-link named @a name to @a location to the group
    @param      location Identifier to object to link to
    @param      name Name of soft-link to create
*/
void Group::create_soft_link(
    Identifier const& location,
    std::string const& name)
{
    auto status = ::H5Lcreate_soft(location.pathname().c_str(), id(),
        name.c_str(), H5P_DEFAULT, H5P_DEFAULT);

    if(status < 0) {
        throw std::runtime_error(fmt::format(
            "Cannot create soft link {} at {}",
            name, location.pathname()
        ));
    }
}


/*!
    @brief      Add a hard-link named @a name to @a location to the group
    @param      location Identifier to object to link to
    @param      name Name of hard-link to create
*/
void Group::create_hard_link(
    Identifier const& location,
    std::string const& name)
{
    auto status = ::H5Lcreate_hard(
        location, location.pathname().c_str(),
        id(), name.c_str(),
        H5P_DEFAULT, H5P_DEFAULT);

    if(status < 0) {
        throw std::runtime_error(fmt::format(
            "Cannot create hard link {} at {}",
            name, location.pathname()
        ));
    }
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
            ::H5Gcreate(
                parent.id(), name.c_str(), H5P_DEFAULT, H5P_DEFAULT,
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
