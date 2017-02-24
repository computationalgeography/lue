#include "lue/cxx_api/hdf5/attributes.h"
#include "lue/c_api/hdf5/hdf5_attribute.h"
#include <cassert>
#include <cstring>


namespace lue {
namespace hdf5 {
// namespace {
// 
// hid_t create_attribute(
//     hid_t location_id,
//     std::string const& name,
//     hid_t type_id,
//     hid_t space_id)
// {
//     return H5Acreate(location_id, name.c_str(), type_id, space_id,
//         H5P_DEFAULT, H5P_DEFAULT);
// }
// 
// 
// hid_t create_string_datatype(
//     size_t nr_bytes)
// {
//     hid_t datatype_id = H5Tcopy(H5T_C_S1);
// 
//     if(datatype_id >= 0) {
//         herr_t status = H5Tset_cset(datatype_id, H5T_CSET_UTF8);
// 
//         if(status < 0) {
//             H5Tclose(datatype_id);
//             return status;
//         }
// 
//         if(nr_bytes > 0) {
//             status = H5Tset_size(datatype_id, nr_bytes);
// 
//             if(status < 0) {
//                 H5Tclose(datatype_id);
//                 return status;
//             }
//         }
//     }
// 
//     return datatype_id;
// }
// 
// 
// template<
//     typename T>
// herr_t             set_attribute       (hid_t location_id,
//                                         std::string const& name,
//                                         T const& value);
// 
// 
// template<>
// herr_t set_attribute(
//     hid_t location_id,
//     std::string const& name,
//     std::string const& value)
// {
//     hid_t datatype_id = create_string_datatype(value.size());
// 
//     if(datatype_id < 0) {
//         return datatype_id;
//     }
// 
// 
//     hid_t dataspace_id = H5Screate(H5S_SCALAR);
// 
//     if(dataspace_id < 0) {
//         H5Tclose(datatype_id);
//         return dataspace_id;
//     }
// 
// 
//     hid_t attribute_id = create_attribute(location_id, name,
//         datatype_id, dataspace_id);
// 
//     if(attribute_id < 0) {
//         H5Sclose(dataspace_id);
//         H5Tclose(datatype_id);
//         return attribute_id;
//     }
// 
// 
//     herr_t status = H5Awrite(attribute_id, datatype_id, value.c_str());
// 
//     if(status < 0) {
//         H5Aclose(attribute_id);
//         H5Sclose(dataspace_id);
//         H5Tclose(datatype_id);
//         return status;
//     }
// 
// 
//     // Wrap-up.
//     status = H5Aclose(attribute_id);
// 
//     if(status < 0) {
//         H5Sclose(dataspace_id);
//         H5Tclose(datatype_id);
//         return status;
//     }
// 
// 
//     status = H5Sclose(dataspace_id);
// 
//     if(status < 0) {
//         H5Tclose(datatype_id);
//         return status;
//     }
// 
// 
//     return H5Tclose(datatype_id);
// }
// 
// 
// template<
//     typename T>
// herr_t             get_attribute       (hid_t location_id,
//                                         std::string const& name,
//                                         T& value);
// 
// 
// template<>
// herr_t get_attribute(
//     hid_t location_id,
//     std::string const& name,
//     std::string& value)
// {
//     hid_t attribute_id = H5Aopen(location_id, name.c_str(), H5P_DEFAULT);
// 
//     if(attribute_id < 0) {
//         return attribute_id;
//     }
// 
// 
//     hid_t datatype_id = H5Aget_type(attribute_id);
// 
//     if(datatype_id < 0) {
//         H5Aclose(attribute_id);
//         return datatype_id;
//     }
// 
//     assert(H5Tget_cset(datatype_id) == H5T_CSET_UTF8);
// 
// 
//     // Reserve space for the character bytes and a trailing '\0'.
//     size_t nr_bytes = H5Tget_size(datatype_id);
//     value.resize(nr_bytes);
// 
//     herr_t status = H5Aread(attribute_id, datatype_id,
//         // Casting not needed anymore in C++17.
//         static_cast<void*>(const_cast<char*>(value.data())));
// 
//     if(status < 0) {
//         H5Tclose(datatype_id);
//         H5Aclose(attribute_id);
//         return status;
//     }
// 
// 
//     // Wrap-up.
//     status = H5Tclose(datatype_id);
// 
//     if(status < 0) {
//         H5Aclose(attribute_id);
//         return status;
//     }
// 
// 
//     return H5Aclose(attribute_id);
// }
// 
// } // Anonymous namespace


Attributes::Attributes(
    Identifier const& location)

    : _location{std::cref(location)}

{
    assert(_location.get().is_valid());
}


template<>
void Attributes::write<std::string>(
    std::string const& name,
    std::string const& value)
{
    assert(_location.get().is_valid());

    auto status = ::hdf5_write_string_attribute(_location.get(),
        name.c_str(), value.c_str());

    if(status < 0) {
        throw std::runtime_error("Cannot write attribute " + name);
    }
}


template<>
std::string Attributes::read<std::string>(
    std::string const& name) const
{
    assert(_location.get().is_valid());

    char* value;

    auto status = ::hdf5_read_string_attribute(_location.get(),
        name.c_str(), &value);

    if(status < 0) {
        throw std::runtime_error("Cannot read attribute " + name);
    }

    std::string result{value};

    ::free(value);

    return result;
}


bool Attributes::exists(
    std::string const& name) const
{
    return hdf5_attribute_exists(_location.get(), name.c_str()) > 0;
}


Identifier Attributes::type_id(
    std::string const& name) const
{
    assert(_location.get().is_valid());
    assert(exists(name));

    Identifier attribute(::H5Aopen(_location.get(), name.c_str(),
        H5P_DEFAULT), ::H5Aclose);
    assert(attribute.is_valid());
    Identifier datatype(::H5Aget_type(attribute), ::H5Tclose);

    if(!datatype.is_valid()) {
        throw std::runtime_error("Datatype cannot be obtained");
    }

    return datatype;
}


#define ATTRIBUTE_METHODS(                                            \
        type,                                                         \
        type_name)                                                    \
template<>                                                            \
void Attributes::create<type>(                                        \
    std::string const& name)                                          \
{                                                                     \
    assert(_location.get().is_valid());                               \
                                                                      \
    auto status = ::hdf5_create_##type_name##_attribute(              \
        _location.get(), name.c_str());                               \
                                                                      \
    if(status < 0) {                                                  \
        throw std::runtime_error("Cannot create attribute " + name);  \
    }                                                                 \
}                                                                     \
                                                                      \
                                                                      \
template<>                                                            \
void Attributes::write<type>(                                         \
    std::string const& name,                                          \
    type const& value)                                                \
{                                                                     \
    assert(_location.get().is_valid());                               \
                                                                      \
    if(!exists(name)) {                                               \
        create<type>(name);                                           \
    }                                                                 \
                                                                      \
    auto status = ::hdf5_write_##type_name##_attribute(               \
        _location.get(), name.c_str(), value);                        \
                                                                      \
    if(status < 0) {                                                  \
        throw std::runtime_error("Cannot write attribute " + name);   \
    }                                                                 \
}                                                                     \
                                                                      \
                                                                      \
template<>                                                            \
type Attributes::read<type>(                                          \
    std::string const& name) const                                    \
{                                                                     \
    assert(_location.get().is_valid());                               \
    assert(exists(name));                                             \
                                                                      \
    type value;                                                       \
                                                                      \
    auto status = ::hdf5_read_##type_name##_attribute(                \
        _location.get(), name.c_str(), &value);                       \
                                                                      \
    if(status < 0) {                                                  \
        throw std::runtime_error("Cannot read attribute " + name);    \
    }                                                                 \
                                                                      \
    return value;                                                     \
}


ATTRIBUTE_METHODS(uint64_t, uint64)
ATTRIBUTE_METHODS(uint32_t, uint32)
ATTRIBUTE_METHODS(uint16_t, uint16)
ATTRIBUTE_METHODS(uint8_t, uint8)
ATTRIBUTE_METHODS(int64_t, int64)
ATTRIBUTE_METHODS(int32_t, int32)
ATTRIBUTE_METHODS(int16_t, int16)
ATTRIBUTE_METHODS(int8_t, int8)
ATTRIBUTE_METHODS(float, float)
ATTRIBUTE_METHODS(double, double)


#undef ATTRIBUTE_METHODS

} // namespace hdf5
} // namespace lue
