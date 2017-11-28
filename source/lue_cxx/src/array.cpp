#include "lue/array.hpp"
#include "lue/hdf5/datatype.hpp"
#include <cassert>


namespace lue {

Array::Array(
    hdf5::Group const& parent,
    std::string const& name,
    hdf5::Datatype const& memory_datatype)

    : hdf5::Dataset{parent, name},
      _memory_datatype{memory_datatype}

{
    assert(id().is_valid());
}


Array::Array(
    hdf5::Dataset&& dataset,
    hdf5::Datatype const& memory_datatype)

    : hdf5::Dataset{std::forward<hdf5::Dataset>(dataset)},
      _memory_datatype{memory_datatype}

{
    assert(id().is_valid());
}


// Array::Array(
//     hdf5::Identifier&& location,
//     hid_t const memory_datatype)
// 
//     : hdf5::Dataset(std::forward<hdf5::Identifier>(location)),
//       _memory_datatype{memory_datatype}
// 
// {
//     std::cout << datatype_as_string(_memory_datatype) << " " << datatype_as_string(this->memory_datatype()) << std::endl;
// }


// Array::Array(
//     hdf5::hdf5::Dataset&& dataset,
//     hid_t const memory_datatype)
// 
//     : hdf5::Dataset(std::forward<hdf5::Dataset>(dataset)),
//       _memory_datatype{memory_datatype}
// 
// {
//     assert(hdf5::is_native_datatype(_memory_datatype));
// }


// Array::Array(
//     hdf5::Identifier const& location,
//     std::string const& name)
// 
//     : hdf5::Dataset(location, name)
// 
// {
//     assert(id().is_valid());
// }


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
    hdf5::Dataspace const& memory_dataspace,
    void const* buffer) const
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


// // Array create_array(
// //     hdf5::Identifier const& location,
// //     std::string const& name,
// //     hid_t const datatype,
// //     hid_t const dataspace)
// // {
// //     return Array(std::move(create_dataset(location, name, datatype,
// //         dataspace)));
// // }
// 
// 
// Array open_array(
//     hdf5::Identifier const& location,
//     std::string const& name)
// {
//     auto dataset = open_dataset(location, name);
//     auto memory_datatype = native_type_id(dataset.memory_datatype());
// 
//     return Array(std::move(dataset), memory_datatype);
// }
// 
// 
// Array open_array(
//     hdf5::Identifier const& location,
//     std::string const& name,
//     hid_t const memory_datatype)
// {
//     return Array(open_dataset(location, name), memory_datatype);
// }

} // namespace lue
