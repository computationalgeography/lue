#include "lue/hdf5/group.h"
#include "lue/hdf5/link.h"
#include <cstring>


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
        throw std::runtime_error("Cannot iterate through links in group at " +
            location.pathname());
    }
}


struct CountObjects
{
    size_t nr_objects;

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


herr_t count_datasets(
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

    if(infobuf.type == H5O_TYPE_DATASET) {
        ++data_->nr_objects;
    }

    return 0;
}


struct ObjectNames
{
    size_t _nr_names;
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
            (char*)malloc((std::strlen(name) + 1) * sizeof(char));
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
            (char*)malloc((std::strlen(name) + 1) * sizeof(char));
        std::strcpy(data_->_names[data_->_nr_names], name);
        ++data_->_nr_names;
    }

    return 0;
}


herr_t retrieve_dataset_names(
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

    if(infobuf.type == H5O_TYPE_DATASET) {
        data_->_names[data_->_nr_names] =
            (char*)malloc((std::strlen(name) + 1) * sizeof(char));
        std::strcpy(data_->_names[data_->_nr_names], name);
        ++data_->_nr_names;
    }

    return 0;
}

}  // Anonymous namespace


bool group_exists(
    Identifier const& location,
    std::string const& name)
{
    return link_exists(location, name) && link_is_group(location, name);
}


/*!
    @brief      Construct a group based on a @a location and a @a name
    @param      location Location of group to open
    @param      name Name of group to open
    @exception  std::runtime_error In case the group cannot be opened
*/
Group::Group(
    Identifier const& location,
    std::string const& name)

    : _id(::H5Gopen(location, name.c_str(), H5P_DEFAULT), ::H5Gclose),
      _attributes(_id)

{
    if(!_id.is_valid()) {
        throw std::runtime_error("Cannot open group " + name);
    }
}


/*!
    @brief      Construct a group based on an identifier
    @param      id Identifier of group
*/
Group::Group(
    Identifier const& id)

    : _id{id},
      _attributes(_id)

{
}


/*!
    @brief      Construct a group based on an existing one
    @param      other Group to copy
*/
Group::Group(
    Group const& other)

    : _id{other._id},
      _attributes{other._attributes}

{
}


/*!
    @brief      Construct a group based on an identifier
    @param      id Identifier of group
*/
Group::Group(
    Identifier&& id)

    : _id{std::forward<Identifier>(id)},
      _attributes(_id)

{
}


Group::Group(
    Group&& other)

    : _id{std::move(other._id)},
      _attributes(_id)

{
    // Invalidate other.
    std::move(other);
}


Group& Group::operator=(
    Group&& other)
{
    _id = std::move(other._id);
    _attributes = Attributes{_id};

    // Invalidate other.
    std::move(other);

    return *this;
}


Identifier const& Group::id() const
{
    return _id;
}


Attributes const& Group::attributes() const
{
    return _attributes;
}


Attributes& Group::attributes()
{
    return _attributes;
}


std::vector<std::string> Group::group_names() const
{
    size_t nr_objects = 0;
    {
        CountObjects data;
        iterate(_id, count_groups, data);
        nr_objects = data.nr_objects;
    }

    auto names = std::make_unique<char*[]>(nr_objects);
    {
        ObjectNames data(names.get());
        iterate(_id, retrieve_group_names, data);
    }

    std::vector<std::string> result(nr_objects);

    for(size_t o = 0; o < nr_objects; ++o) {
        result[o] = names[o];
        std::free(names[o]);
    }

    return result;
}


bool Group::contains_group(
    std::string const& name) const
{
    return group_exists(id(), name);
}


bool Group::contains_dataset(
    std::string const& name) const
{
    return dataset_exists(id(), name);
}


bool Group::contains_attribute(
    std::string const& name) const
{
    return _attributes.exists(name);
}


bool Group::contains_soft_link(
    std::string const& name) const
{
    return soft_link_exists(id(), name);
}


void Group::create_soft_link(
    Identifier const& target,
    std::string const& name)
{
    auto status = ::H5Lcreate_soft(target.pathname().c_str(), id(),
        name.c_str(), H5P_DEFAULT, H5P_DEFAULT);

    if(status < 0) {
        throw std::runtime_error("Cannot create softlink " + name +
            " at " + target.pathname());
    }
}


bool operator==(
    Group const& lhs,
    Group const& rhs)
{
    H5O_info_t lhs_info;
    auto status = ::H5Oget_info(lhs.id(), &lhs_info);

    if(status < 0) {
        throw std::runtime_error("Cannot retrieve metadata for group " +
            lhs.id().pathname());
    }

    H5O_info_t rhs_info;
    status = H5Oget_info(rhs.id(), &rhs_info);

    if(status < 0) {
        throw std::runtime_error("Cannot retrieve metadata for group " +
            rhs.id().pathname());
    }

    return
        lhs_info.fileno == rhs_info.fileno &&
        lhs_info.addr == rhs_info.addr
        ;
}


bool operator!=(
    Group const& lhs,
    Group const& rhs)
{
    return !(lhs == rhs);
}


// Group open_group(
//     Group const& group,
//     std::string const& name)
// {
//     return open_group(group.id(), name);
// }
// 
// 
// Group open_group(
//     Identifier const& id,
//     std::string const& name)
// {
//     Identifier group_id(::H5Gopen(id, name.c_str(), H5P_DEFAULT),
//         ::H5Gclose);
// 
//     if(!group_id.is_valid()) {
//         throw std::runtime_error("Cannot open group " + name +
//             " at " + id.pathname());
//     }
// 
//     return Group(std::move(group_id));
// }


Group create_group(
    Identifier const& id,
    std::string const& name)
{
    if(group_exists(id, name)) {
        throw std::runtime_error("Group " + name + " already exists");
    }

    Identifier group_id(::H5Gcreate(id, name.c_str(), H5P_DEFAULT,
        H5P_DEFAULT, H5P_DEFAULT), H5Gclose);

    if(!group_id.is_valid()) {
        throw std::runtime_error("Cannot create group " + name + " at " +
            id.pathname());
    }

    return Group(std::move(group_id));
}


std::vector<std::string> object_names(
    Group const& group)
{
    size_t nr_objects = 0;
    {
        CountObjects data;
        iterate(group.id(), count_objects, data);
        nr_objects = data.nr_objects;
    }

    auto names = std::make_unique<char*[]>(nr_objects);
    {
        ObjectNames data(names.get());
        iterate(group.id(), retrieve_object_names, data);
    }

    std::vector<std::string> result(nr_objects);

    for(size_t o = 0; o < nr_objects; ++o) {
        result[o] = names[o];
        std::free(names[o]);
    }

    return result;
}


// std::vector<std::string> group_names(
//     Group const& group)
// {
//     size_t nr_objects = 0;
//     {
//         CountObjects data;
//         iterate(group.id(), count_groups, data);
//         nr_objects = data.nr_objects;
//     }
// 
//     auto names = std::make_unique<char*[]>(nr_objects);
//     {
//         ObjectNames data(names.get());
//         iterate(group.id(), retrieve_group_names, data);
//     }
// 
//     std::vector<std::string> result(nr_objects);
// 
//     for(size_t o = 0; o < nr_objects; ++o) {
//         result[o] = names[o];
//         std::free(names[o]);
//     }
// 
//     return result;
// }


std::vector<std::string> dataset_names(
    Group const& group)
{
    size_t nr_objects = 0;
    {
        CountObjects data;
        iterate(group.id(), count_datasets, data);
        nr_objects = data.nr_objects;
    }

    auto names = std::make_unique<char*[]>(nr_objects);
    {
        ObjectNames data(names.get());
        iterate(group.id(), retrieve_dataset_names, data);
    }

    std::vector<std::string> result(nr_objects);

    for(size_t o = 0; o < nr_objects; ++o) {
        result[o] = names[o];
        std::free(names[o]);
    }

    return result;
}


// Dataset open_dataset(
//     Group const& group,
//     std::string const& name)
// {
//     return open_dataset(group.id(), name);
// }

} // namespace hdf5
} // namespace lue
