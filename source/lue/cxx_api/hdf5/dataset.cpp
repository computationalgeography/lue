#include "lue/cxx_api/hdf5/dataset.h"
#include <cassert>
// #include <iostream>


namespace lue {
namespace hdf5 {

Dataset::Dataset(
    Identifier&& id)

    : _id{std::forward<Identifier>(id)}

{
}


Identifier const& Dataset::id() const
{
    return _id;
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
    hsize_t const start,
    hsize_t const count,
    hsize_t const stride,
    void* buffer) const
{
    // Select elements: create hyperslab
    auto dataspace = this->dataspace();
    hsize_t const* block = nullptr;
    auto status = H5Sselect_hyperslab(dataspace.id(), H5S_SELECT_SET, &start,
        &stride, &count, block);

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
    hsize_t const start,
    hsize_t const count,
    hsize_t const stride,
    void const* buffer) const
{
    // Select elements: create hyperslab
    auto dataspace = this->dataspace();
    hsize_t const* block = nullptr;
    auto status = H5Sselect_hyperslab(dataspace.id(), H5S_SELECT_SET, &start,
        &stride, &count, block);

    if(status < 0) {
        throw std::runtime_error("Cannot create hyperslab");
    }

    status = H5Dwrite(_id, type_id, H5S_ALL, dataspace.id(), H5P_DEFAULT,
        buffer);

    if(status < 0) {
        throw std::runtime_error("Cannot write to dataset");
    }
}

} // namespace hdf5
} // namespace lue
