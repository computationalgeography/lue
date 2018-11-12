#include "lue/hdf5/dataset.hpp"
#include "lue/hdf5/link.hpp"
#include <fmt/format.h>
#include <cassert>
#include <cstring>
#include <memory>


namespace lue {
namespace hdf5 {

Dataset::CreationPropertyList::CreationPropertyList()

    : PropertyList(H5P_DATASET_CREATE)

{
}


void Dataset::CreationPropertyList::set_chunk(
    Shape const& chunk)
{
    auto status =
        ::H5Pset_chunk(id(), static_cast<int>(chunk.size()), chunk.data());

    if(status < 0) {
        throw std::runtime_error("Cannot set chunk size");
    }
}


Dataset::TransferPropertyList::TransferPropertyList()

    : PropertyList(H5P_DATASET_XFER)

{
}


#ifdef HDF5_IS_PARALLEL

/*!
    @brief      Set data transfer mode
    @exception  std::runtime_error In case data transfer mode cannot be set
    @sa         [H5Pset_dxpl_mpio](https://support.hdfgroup.org/HDF5/doc/RM/RM_H5P.html#Property-SetDxplMpio)
*/
void Dataset::TransferPropertyList::set_transfer_mode(
    H5FD_mpio_xfer_t const xfer_mode)
{
    auto status = ::H5Pset_dxpl_mpio(id(), xfer_mode);

    if(status < 0) {
        throw std::runtime_error("Cannot set data transfer mode");
    }
}

#endif


/*!
    @brief      Construct a dataset instance based on an existing
                HDF5 dataset
    @exception  std::runtime_error In case dataset named @a name and
                located at @a parent cannot be opened
*/
Dataset::Dataset(
    Group& parent,
    std::string const& name)

    : PrimaryDataObject{Identifier{
        ::H5Dopen(parent.id(), name.c_str(), H5P_DEFAULT), ::H5Dclose}}

{
    if(!id().is_valid()) {
        throw std::runtime_error(fmt::format(
                "Cannot open dataset {} at {}",
                name, parent.id().pathname()
            ));
    }

    assert(id().is_valid());
}


Dataset::Dataset(
    Identifier& id)

    : PrimaryDataObject{id}

{
    assert(this->id().is_valid());
}


Dataset::Dataset(
    Identifier&& id)

    : PrimaryDataObject{std::forward<Identifier>(id)}

{
    assert(this->id().is_valid());
}


/*!
    @brief      Return the datatype of the dataset

    The datatype returned is the in-file datatype of the dataset
*/
Datatype Dataset::datatype() const
{
    return Datatype(Identifier(::H5Dget_type(id()), ::H5Tclose));
}


void Dataset::resize(
    Shape const& new_dimension_sizes)
{
    assert(static_cast<int>(new_dimension_sizes.size()) ==
        dataspace().nr_dimensions());

    auto status = ::H5Dset_extent(id(), new_dimension_sizes.data());

    if(status < 0) {
        throw std::runtime_error("Cannot resize dataset");
    }
}


Dataspace Dataset::dataspace() const
{
    assert(id().is_valid());

    return Dataspace(::H5Dget_space(id()));
}


Shape Dataset::shape() const
{
    assert(id().is_valid());

    return dataspace().dimension_extents();
}


void Dataset::read(
    Datatype const& datatype,
    void* buffer) const
{
    read(datatype, Hyperslab(shape()), buffer);
}


void Dataset::read(
    Datatype const& datatype,
    Hyperslab const& hyperslab,
    void* buffer) const
{
    auto const memory_dataspace = create_dataspace(
        Shape(hyperslab.count().begin(), hyperslab.count().end()));

    read(datatype, memory_dataspace, hyperslab, buffer);
}


void Dataset::read(
    Datatype const& datatype,
    Dataspace const& memory_dataspace,
    void* buffer) const
{
    read(datatype, memory_dataspace, Hyperslab(shape()), buffer);
}


void Dataset::read(
    Datatype const& datatype,
    Dataspace const& memory_dataspace,
    Hyperslab const& hyperslab,
    void* buffer) const
{
    assert(datatype.is_native() || datatype.is_string());

    // Select elements: create hyperslab
    auto const file_dataspace = this->dataspace();
    hsize_t const* block = nullptr;
    auto status = ::H5Sselect_hyperslab(file_dataspace.id(), H5S_SELECT_SET,
        hyperslab.start().data(), hyperslab.stride().data(),
        hyperslab.count().data(), block);

    if(status < 0) {
        throw std::runtime_error("Cannot create hyperslab");
    }

    assert(id().is_valid());
    assert(file_dataspace.id().is_valid());

    status = ::H5Dread(
        id(), datatype.id(),
        memory_dataspace.id(), file_dataspace.id(),
        H5P_DEFAULT, buffer);

    if(status < 0) {
        throw std::runtime_error("Cannot read from dataset");
    }
}


/*!
    @brief      Write buffer to underlying HDF5 dataset
    @param      datatype Data type of elements in @a buffer
    @param      buffer Buffer with elements for whole dataset

    In case the dataset contains values for multiple items, it is assumed
    that it is large enough for all elements.
*/
void Dataset::write(
    Datatype const& datatype,
    void const* buffer) const
{
    // Select all values in the dataset. Assume the buffer contains
    // values for all values in the dataset.
    write(datatype, Hyperslab(shape()), buffer);
}


void Dataset::write(
    Datatype const& datatype,
    Hyperslab const& hyperslab,
    void const* buffer) const
{
    // Assume values in memory are layed out contiguously in all dimensions
    auto const memory_dataspace = create_dataspace(
        Shape(hyperslab.count().begin(), hyperslab.count().end()));

    write(datatype, memory_dataspace, hyperslab, buffer);
}


void Dataset::write(
    Datatype const& datatype,
    Dataspace const& memory_dataspace,
    void const* buffer) const
{
    write(datatype, memory_dataspace, Hyperslab(shape()), buffer);
}


/*!
    @brief      .
    @param      hyperslab Selection of file dataspace to write to
    @exception  .
*/
void Dataset::write(
    Datatype const& datatype,
    Dataspace const& memory_dataspace,
    Hyperslab const& hyperslab,
    void const* buffer) const
{
    assert(datatype.is_native() || datatype.is_string());

    // Select elements: create hyperslab
    auto file_dataspace = this->dataspace();
    hsize_t const* block = nullptr;
    auto status = ::H5Sselect_hyperslab(file_dataspace.id(), H5S_SELECT_SET,
        hyperslab.start().data(), hyperslab.stride().data(),
        hyperslab.count().data(), block);

    if(status < 0) {
        throw std::runtime_error("Cannot create hyperslab");
    }

    status = ::H5Dwrite(
        id(), datatype.id(),
        memory_dataspace.id(), file_dataspace.id(),
        H5P_DEFAULT, buffer);

    if(status < 0) {
        throw std::runtime_error("Cannot write to dataset");
    }
}


/*!
    @brief      Fill the whole dataset with a single value
    @param      datatype In-memory datatype of fill value
    @param      hyperslab Selection of dataset to fill
    @param      buffer Pointer to fill value
    @exception  std::runtime_error In case the dataset cannot be filled
*/
void Dataset::fill(
    Datatype const& datatype,
    Hyperslab const& hyperslab,
    void const* buffer) const
{
    assert(datatype.is_native());

    // We have to fill the dataset with a single value. But to do this
    // it seems we have to have a memory buffer with the same number of
    // values that we want to write to. There is no fill functionality
    // in the HDF5 API.

    // Create a buffer to hold the fill values.
    using byte = unsigned char;
    static_assert(sizeof(byte) == 1, "");

    std::size_t const nr_elements = hyperslab.nr_elements();
    std::size_t const nr_bytes_per_element = datatype.size();
    std::size_t const nr_bytes = nr_elements * nr_bytes_per_element;
    auto memory_buffer = std::make_unique<byte[]>(nr_bytes);

    // Fill the buffer with the fill value.
    {
        auto dst = memory_buffer.get();

        for(std::size_t i = 0; i < nr_elements; ++i,
                dst += nr_bytes_per_element) {
            std::memcpy(dst, buffer, datatype.size());
        }
    }

    auto const memory_dataspace = create_dataspace(Shape{nr_elements});

    write(datatype, memory_dataspace, hyperslab, memory_buffer.get());
}


bool dataset_exists(
    Identifier const& parent,
    std::string const& name)
{
    return link_exists(parent, name) && link_is_dataset(parent, name);
}


Dataset open_dataset(
    Identifier& parent,
    std::string const& name)
{
    Identifier dataset_location(::H5Dopen(parent, name.c_str(),
        H5P_DEFAULT), ::H5Dclose);

    if(!dataset_location.is_valid()) {
        throw std::runtime_error(fmt::format(
                "Cannot open dataset {}",
                name
            ));
    }

    return Dataset(std::move(dataset_location));
}


/*!
    @brief      Add an HDF5 dataset and return the corresponding Dataset
                instance
    @param      parent Id of parent object of dataset
    @param      name Name of dataset
    @param      datatype Datatype of elements in dataset
    @param      dataspace Dataspace of dataset
    @param      creation_property_list Creation properties
    @return     Newly created dataset
    @exception  std::runtime_error In case dataset already exists,
                or cannot be created
*/
Dataset create_dataset(
    Identifier& parent,
    std::string const& name,
    Datatype const& datatype,
    Dataspace const& dataspace,
    Dataset::CreationPropertyList const& creation_property_list)
{
    assert(datatype.is_standard() || datatype.is_string());

    if(dataset_exists(parent, name)) {
        throw std::runtime_error(fmt::format(
                "Dataset {} already exists",
                name
            ));
    }

    Identifier dataset_location(::H5Dcreate(parent, name.c_str(),
        datatype.id(), dataspace.id(), H5P_DEFAULT,
        creation_property_list.id(), H5P_DEFAULT), ::H5Dclose);

    if(!dataset_location.is_valid()) {
        throw std::runtime_error(fmt::format(
                "Cannot create dataset {}",
                name
            ));
    }

    return Dataset(std::move(dataset_location));
}

} // namespace hdf5
} // namespace lue
