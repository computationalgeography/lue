#include "lue/cxx_api/hdf5/dataset.h"
#include "lue/cxx_api/hdf5/datatype.h"
#include "lue/c_api/hdf5/hdf5_dataset.h"
#include <cassert>
#include <iostream>


namespace lue {
namespace hdf5 {

/*!
    @ingroup    lue_cxx_api_hdf5_group
*/
bool dataset_exists(
    hdf5::Identifier const& location,
    std::string const& name)
{
    return ::hdf5_dataset_exists(location, name.c_str()) > 0;
}


Dataset::Dataset(
    Identifier&& id)

    : _id{std::forward<Identifier>(id)}

{
    assert(_id.is_valid());
}


Identifier const& Dataset::id() const
{
    return _id;
}


hdf5::Identifier Dataset::type_id() const
{
    Identifier datatype_location(::H5Dget_type(_id), ::H5Tclose);

    if(!datatype_location.is_valid()) {
        throw std::runtime_error("Datatype cannot be obtained");
    }

    return datatype_location;
}


void Dataset::resize(
    std::vector<hsize_t> const& new_dimension_sizes)
{
    assert(static_cast<int>(new_dimension_sizes.size()) ==
        dataspace().nr_dimensions());

    auto status = ::H5Dset_extent(_id, new_dimension_sizes.data());

    if(status < 0) {
        throw std::runtime_error("Dataset cannot be resized");
    }
}


Dataspace Dataset::dataspace() const
{
    Identifier dataspace_location(::H5Dget_space(_id), ::H5Sclose);

    if(!dataspace_location.is_valid()) {
        throw std::runtime_error("Dataspace cannot be opened");
    }

    return Dataspace{std::move(dataspace_location)};
}


void Dataset::read(
    hid_t const type_id,
    std::vector<hsize_t> const& start,
    std::vector<hsize_t> const& count,
    std::vector<hsize_t> const& stride,
    void* buffer) const
{
    assert(is_native_datatype(type_id));

    // Select elements: create hyperslab
    auto dataspace = this->dataspace();
    hsize_t const* block = nullptr;
    auto status = H5Sselect_hyperslab(dataspace.id(), H5S_SELECT_SET,
        start.data(), stride.data(), count.data(), block);

    if(status < 0) {
        throw std::runtime_error("Cannot create hyperslab");
    }

    status = H5Dread(_id, type_id, H5S_ALL, dataspace.id(), H5P_DEFAULT,
        buffer);

    if(status < 0) {
        throw std::runtime_error("Cannot read from dataset");
    }
}


void Dataset::write(
    hid_t const type_id,
    std::vector<hsize_t> const& start,
    std::vector<hsize_t> const& count,
    std::vector<hsize_t> const& stride,
    void const* buffer) const
{
    assert(is_native_datatype(type_id));

    // Select elements: create hyperslab
    auto dataspace = this->dataspace();
    hsize_t const* block = nullptr;
    auto status = H5Sselect_hyperslab(dataspace.id(), H5S_SELECT_SET,
        start.data(), stride.data(), count.data(), block);

    if(status < 0) {
        throw std::runtime_error("Cannot create hyperslab");
    }

    status = H5Dwrite(_id, type_id, H5S_ALL, dataspace.id(), H5P_DEFAULT,
        buffer);

    if(status < 0) {
        throw std::runtime_error("Cannot write to dataset");
    }
}


Dataset open_dataset(
    hdf5::Identifier const& location,
    std::string const& name)
{
    hdf5::Identifier dataset_location(::hdf5_open_dataset(location,
        name.c_str()), ::hdf5_close_dataset);

    if(!dataset_location.is_valid()) {
        throw std::runtime_error("Dataset " + name + " cannot be opened");
    }

    return Dataset(std::move(dataset_location));
}


Dataset create_dataset(
    hdf5::Identifier const& location,
    std::string const& name,
    hid_t const datatype,
    Dataspace const& dataspace,
    hid_t const creation_property_list)
{
    assert(is_standard_datatype(datatype));

    if(dataset_exists(location, name)) {
        throw std::runtime_error("Dataset " + name + " already exists");
    }

    hdf5::Identifier dataset_location(::H5Dcreate(location, name.c_str(),
        datatype, dataspace.id(), H5P_DEFAULT, creation_property_list,
        H5P_DEFAULT), ::H5Dclose);

    if(!dataset_location.is_valid()) {
        throw std::runtime_error("Dataset " + name + " cannot be created");
    }

    return Dataset(std::move(dataset_location));
}

} // namespace hdf5
} // namespace lue
