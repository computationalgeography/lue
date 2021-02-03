#include "lue/core/array.hpp"
#include <cassert>


namespace lue {
namespace data_model {

Array::Array(
    hdf5::Group const& parent,
    std::string const& name):

    hdf5::Dataset{parent, name},
    _memory_datatype{hdf5::memory_datatype(datatype())}

{
    assert(id().is_valid());
}


Array::Array(
    hdf5::Group const& parent,
    std::string const& name,
    hdf5::Datatype const& memory_datatype):

    hdf5::Dataset{parent, name},
    _memory_datatype{memory_datatype}

{
    assert(id().is_valid());
}


Array::Array(
    hdf5::Dataset&& dataset,
    hdf5::Datatype const& memory_datatype):

    hdf5::Dataset{std::move(dataset)},
    _memory_datatype{memory_datatype}

{
    assert(id().is_valid());
}


Array::Array(
    hdf5::Identifier&& id,
    hdf5::Datatype const& memory_datatype):

    hdf5::Dataset{std::move(id)},
    _memory_datatype{memory_datatype}

{
    assert(this->id().is_valid());
}


hdf5::Datatype const& Array::memory_datatype() const
{
    return _memory_datatype;
}


hdf5::Datatype Array::file_datatype() const
{
    return datatype();
}


void Array::read(
    void* buffer) const
{
    hdf5::Dataset::read(_memory_datatype, buffer);
}


void Array::read(
    hdf5::Hyperslab const& hyperslab,
    void* buffer) const
{
    hdf5::Dataset::read(_memory_datatype, hyperslab, buffer);
}


void Array::read(
    hdf5::Datatype const& memory_datatype,
    hdf5::Hyperslab const& hyperslab,
    void* buffer) const
{
    hdf5::Dataset::read(memory_datatype, hyperslab, buffer);
}


void Array::read(
    hdf5::Datatype const& memory_datatype,
    hdf5::Hyperslab const& hyperslab,
    TransferPropertyList const& transfer_property_list,
    void* buffer) const
{
    hdf5::Dataset::read(memory_datatype, hyperslab, transfer_property_list, buffer);
}


void Array::read(
    hdf5::Dataspace const& memory_dataspace,
    void* buffer) const
{
    hdf5::Dataset::read(_memory_datatype, memory_dataspace, buffer);
}


void Array::write(
    void const* buffer)
{
    hdf5::Dataset::write(_memory_datatype, buffer);
}


void Array::write(
    hdf5::Hyperslab const& hyperslab,
    void const* buffer)
{
    hdf5::Dataset::write(_memory_datatype, hyperslab, buffer);
}


void Array::write(
    hdf5::Hyperslab const& hyperslab,
    TransferPropertyList const& transfer_property_list,
    void const* buffer)
{
    hdf5::Dataset::write(_memory_datatype, hyperslab, transfer_property_list, buffer);
}


void Array::write(
    hdf5::Dataspace const& memory_dataspace,
    void const* buffer)
{
    hdf5::Dataset::write(_memory_datatype, memory_dataspace, buffer);
}


void Array::write(
    hdf5::Dataspace const& memory_dataspace,
    hdf5::Hyperslab const& hyperslab,
    void const* buffer)
{
    hdf5::Dataset::write(_memory_datatype, memory_dataspace, hyperslab, buffer);
}


void Array::write(
    hdf5::Dataspace const& memory_dataspace,
    hdf5::Hyperslab const& hyperslab,
    TransferPropertyList const& transfer_property_list,
    void const* buffer)
{
    hdf5::Dataset::write(_memory_datatype, memory_dataspace, hyperslab, transfer_property_list, buffer);
}

}  // namespace data_model
}  // namespace lue
