#include "lue/array.h"
#include "lue/hdf5/datatype.h"
#include <cassert>


namespace lue {

// bool array_exists(
//     hdf5::Identifier const& location,
//     std::string const& name)
// {
//     return hdf5::dataset_exists(location, name);
// }


Array::Array(
    hdf5::Dataset&& dataset,
    hdf5::Datatype const& memory_datatype)

    : Dataset(std::forward<hdf5::Dataset>(dataset)),
      _memory_datatype{memory_datatype}

{
}


Array::Array(
    hdf5::Identifier const& location,
    std::string const& name,
    hdf5::Datatype const& memory_datatype)

    : Dataset(location, name),
      _memory_datatype{memory_datatype}

{
}


// Array::Array(
//     hdf5::Identifier&& location,
//     hid_t const memory_datatype)
// 
//     : Dataset(std::forward<hdf5::Identifier>(location)),
//       _memory_datatype{memory_datatype}
// 
// {
//     std::cout << datatype_as_string(_memory_datatype) << " " << datatype_as_string(this->memory_datatype()) << std::endl;
// }


// Array::Array(
//     hdf5::Dataset&& dataset,
//     hid_t const memory_datatype)
// 
//     : Dataset(std::forward<hdf5::Dataset>(dataset)),
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


hdf5::Shape Array::shape() const
{
    assert(id().is_valid());

    return dataspace().dimension_extents();
}


void Array::read(
    hdf5::Offset const& start,
    hdf5::Stride const& stride,
    hdf5::Count const& count,
    void* buffer) const
{
    Dataset::read(_memory_datatype, start, stride, count, buffer);
}


// void Array::write(
//     extent_t const count,
//     void const* buffer)
// {
//     write({0}, {count}, {1}, buffer);
// }
// 
// 
// void Array::write(
//     std::vector<extent_t> const count,
//     void const* buffer)
// {
//     std::vector<extent_t> start(count.size(), 0);
//     std::vector<extent_t> stride(count.size(), 1);
// 
//     write(start, count, stride, buffer);
// }


void Array::write(
    hdf5::Offset const& start,
    hdf5::Stride const& stride,
    hdf5::Count const& count,
    void const* buffer)
{
    Dataset::write(_memory_datatype, start, stride, count, buffer);
}


// void Array::write(
//     hdf5::Dataspace const& memory_dataspace,
//     std::vector<extent_t> const& start,
//     std::vector<extent_t> const& count,
//     std::vector<extent_t> const& stride,
//     void const* buffer)
// {
//     Dataset::write(_memory_datatype, memory_dataspace, start, count, stride, buffer);
// }
// 
// 
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
