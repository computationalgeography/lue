#include "lue/cxx_api/hdf5/group.h"
#include "lue/c_api/hdf5/hdf5_group.h"
#include "lue/c_api/hdf5/hdf5_link.h"
#include "lue/cxx_api/hdf5/link.h"


namespace lue {
namespace hdf5 {

/*!
    @ingroup    lue_cxx_api_group
*/
bool group_exists(
    Identifier const& location,
    std::string const& name)
{
    return ::hdf5_group_exists(location, name.c_str()) > 0;
}


Group::Group(
    Identifier const& location,
    std::string const& name)

    : _id(::hdf5_open_group(location, name.c_str()), ::hdf5_close_group),
      _attributes{_id}

{
    if(!_id.is_valid()) {
        throw std::runtime_error("Group " + name + " cannot be opened");
    }
}


Group::Group(
    Identifier&& id)

    : _id{std::forward<Identifier>(id)},
      _attributes{_id}

{
    assert(_id.is_valid());
}


Group::Group(
    Group&& other)

    : _id{std::move(other._id)},
      _attributes{_id}

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


std::string Group::name() const
{
    return _id.name();
}


Attributes const& Group::attributes() const
{
    return _attributes;
}


Attributes& Group::attributes()
{
    return _attributes;
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
    auto status = ::hdf5_create_soft_link(target, name.c_str(), id());

    if(status < 0) {
        throw std::runtime_error("Soft link cannot be created");
    }
}


bool operator==(
    Group const& lhs,
    Group const& rhs)
{
    H5O_info_t lhs_info;
    auto status = H5Oget_info(lhs.id(), &lhs_info);

    if(status < 0) {
        return false;
    }

    H5O_info_t rhs_info;
    status = H5Oget_info(rhs.id(), &rhs_info);

    if(status < 0) {
        return false;
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


Group open_group(
    Group const& group,
    std::string const& name)
{
    return open_group(group.id(), name);
}


Group open_group(
    Identifier const& id,
    std::string const& name)
{
    Identifier group_id(
            ::H5Gopen(id, name.c_str(), H5P_DEFAULT),
            ::H5Gclose);

    if(!group_id.is_valid()) {
        throw std::runtime_error("Group " + name + " cannot be opened");
    }

    return Group(std::move(group_id));
}


/*!
    @ingroup    lue_cxx_api_group
*/
Group create_group(
    Identifier const& id,
    std::string const& name)
{
    if(group_exists(id, name)) {
        throw std::runtime_error("Group " + name + " already exists");
    }

    Identifier group_id(::hdf5_create_group(id, name.c_str()),
        ::hdf5_close_group);

    if(!group_id.is_valid()) {
        throw std::runtime_error("Group " + name + " cannot be created");
    }

    return Group(std::move(group_id));
}


std::vector<std::string> group_names(
    Group const& group)
{
    auto const nr_groups = hdf5_nr_groups(group.id());

    char* names[nr_groups];

    auto status = hdf5_group_names(group.id(), names);

    if(status < 0) {
        throw std::runtime_error("Cannot determine group names");
    }

    std::vector<std::string> result(nr_groups);

    for(size_t g = 0; g < nr_groups; ++g) {
        result[g] = names[g];
        free(names[g]);
    }

    return result;
}


std::vector<std::string> dataset_names(
    Group const& group)
{
    auto const nr_datasets = hdf5_nr_datasets(group.id());

    char* names[nr_datasets];

    auto status = hdf5_dataset_names(group.id(), names);

    if(status < 0) {
        throw std::runtime_error("Cannot determine dataset names");
    }

    std::vector<std::string> result(nr_datasets);

    for(size_t d = 0; d < nr_datasets; ++d) {
        result[d] = names[d];
        free(names[d]);
    }

    return result;
}


Dataset open_dataset(
    hdf5::Group const& group,
    std::string const& name)
{
    return open_dataset(group.id(), name);
}

} // namespace hdf5
} // namespace lue
