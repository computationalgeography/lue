#include "lue/hdf5/dataset.h"
#include "lue/hdf5/datatype.h"
#include "lue/hdf5/link.h"
#include <cassert>
#include <iostream>


namespace lue {
namespace hdf5 {

bool dataset_exists(
    Identifier const& location,
    std::string const& name)
{
    return link_exists(location, name) && link_is_dataset(location, name);
}


Dataset::Dataset(
    Identifier const& location,
    std::string const& name)

    : _id(::H5Dopen(location, name.c_str(), H5P_DEFAULT), ::H5Dclose)

{
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


Datatype Dataset::datatype() const
{
    return Datatype(Identifier(::H5Dget_type(_id), ::H5Tclose));
}


void Dataset::resize(
    Shape const& new_dimension_sizes)
{
    assert(static_cast<int>(new_dimension_sizes.size()) ==
        dataspace().nr_dimensions());

    auto status = ::H5Dset_extent(_id, new_dimension_sizes.data());

    if(status < 0) {
        throw std::runtime_error("Cannot resize dataset");
    }
}


Dataspace Dataset::dataspace() const
{
    assert(_id.is_valid());

    return Dataspace(::H5Dget_space(_id));
}


void Dataset::read(
    Datatype const& datatype,
    Offset const& start,
    Stride const& stride,
    Count const& count,
    void* buffer) const
{
    assert(datatype.is_native());

    // Select elements: create hyperslab
    auto const dataspace = this->dataspace();
    hsize_t const* block = nullptr;
    auto status = ::H5Sselect_hyperslab(dataspace.id(), H5S_SELECT_SET,
        start.data(), stride.data(), count.data(), block);

    if(status < 0) {
        throw std::runtime_error("Cannot create hyperslab");
    }

    status = ::H5Dread(_id, datatype.id(), H5S_ALL, dataspace.id(),
        H5P_DEFAULT, buffer);

    if(status < 0) {
        throw std::runtime_error("Cannot read from dataset");
    }
}


void Dataset::write(
    Datatype const& datatype,
    Offset const& start,
    Stride const& stride,
    Count const& count,
    void const* buffer) const
{
    assert(datatype.is_native());

    // Select elements: create hyperslab
    auto const dataspace = this->dataspace();
    hsize_t const* block = nullptr;
    auto status = ::H5Sselect_hyperslab(dataspace.id(), H5S_SELECT_SET,
        start.data(), stride.data(), count.data(), block);

    if(status < 0) {
        throw std::runtime_error("Cannot create hyperslab");
    }

    status = H5Dwrite(_id, datatype.id(), H5S_ALL, dataspace.id(),
        H5P_DEFAULT, buffer);

    if(status < 0) {
        throw std::runtime_error("Cannot write to dataset");
    }
}


void Dataset::write(
    Datatype const& datatype,
    Dataspace const& memory_dataspace,
    Offset const& start,
    Stride const& stride,
    Count const& count,
    void const* buffer) const
{
    assert(datatype.is_native());

    // Select elements: create hyperslab
    auto file_dataspace = this->dataspace();
    hsize_t const* block = nullptr;
    auto status = ::H5Sselect_hyperslab(file_dataspace.id(), H5S_SELECT_SET,
        start.data(), stride.data(), count.data(), block);

    if(status < 0) {
        throw std::runtime_error("Cannot create hyperslab");
    }

    status = ::H5Dwrite(_id, datatype.id(), memory_dataspace.id(),
        file_dataspace.id(), H5P_DEFAULT, buffer);

    if(status < 0) {
        throw std::runtime_error("Cannot write to dataset");
    }
}


Dataset open_dataset(
    Identifier const& location,
    std::string const& name)
{
    Identifier dataset_location(::H5Dopen(location, name.c_str(),
        H5P_DEFAULT), ::H5Dclose);

    if(!dataset_location.is_valid()) {
        throw std::runtime_error("Cannot open dataset " + name);
    }

    return Dataset(std::move(dataset_location));
}


Dataset create_dataset(
    Identifier const& location,
    std::string const& name,
    Datatype const& datatype,
    Dataspace const& dataspace,
    hid_t const creation_property_list)
{
    assert(datatype.is_standard());

    if(dataset_exists(location, name)) {
        throw std::runtime_error("Dataset " + name + " already exists");
    }

    Identifier dataset_location(::H5Dcreate(location, name.c_str(),
        datatype.id(), dataspace.id(), H5P_DEFAULT, creation_property_list,
        H5P_DEFAULT), ::H5Dclose);

    if(!dataset_location.is_valid()) {
        throw std::runtime_error("Cannot create dataset " + name);
    }

    return Dataset(std::move(dataset_location));
}

} // namespace hdf5
} // namespace lue
