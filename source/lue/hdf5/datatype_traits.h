#pragma once
#include <hdf5.h>
#include <string>


namespace lue {
namespace hdf5 {

template<
    typename T>
struct NativeDatatypeTraits
{
};

#define NATIVE_DATATYPE_TRAITS(  \
        type,                    \
        type_id_)                \
template<>                       \
struct NativeDatatypeTraits<type>      \
{                                             \
    static hid_t type_id() { return type_id_; }  \
    static std::string name() { return #type_id_; } \
};

NATIVE_DATATYPE_TRAITS(int8_t, H5T_NATIVE_INT8)
NATIVE_DATATYPE_TRAITS(int16_t, H5T_NATIVE_INT16)
NATIVE_DATATYPE_TRAITS(int32_t, H5T_NATIVE_INT32)
NATIVE_DATATYPE_TRAITS(int64_t, H5T_NATIVE_INT64)
NATIVE_DATATYPE_TRAITS(uint8_t, H5T_NATIVE_UINT8)
NATIVE_DATATYPE_TRAITS(uint16_t, H5T_NATIVE_UINT16)
NATIVE_DATATYPE_TRAITS(uint32_t, H5T_NATIVE_UINT32)
NATIVE_DATATYPE_TRAITS(uint64_t, H5T_NATIVE_UINT64)
// TODO Exclude on macOS
// NATIVE_DATATYPE_TRAITS(hsize_t, H5T_NATIVE_HSIZE)
NATIVE_DATATYPE_TRAITS(float, H5T_NATIVE_FLOAT)
NATIVE_DATATYPE_TRAITS(double, H5T_NATIVE_DOUBLE)

#undef NATIVE_DATATYPE_TRAITS


template<
    typename T>
struct StandardDatatypeTraits
{
};

#define STANDARD_DATATYPE_TRAITS(  \
        type,                    \
        type_id_)                \
template<>                       \
struct StandardDatatypeTraits<type>      \
{                                             \
    static hid_t type_id() { return type_id_; }  \
    static std::string name() { return #type_id_; } \
};

STANDARD_DATATYPE_TRAITS(int8_t,  H5T_STD_I8LE)
STANDARD_DATATYPE_TRAITS(int16_t,  H5T_STD_I16LE)
STANDARD_DATATYPE_TRAITS(int32_t,  H5T_STD_I32LE)
STANDARD_DATATYPE_TRAITS(int64_t,  H5T_STD_I64LE)
STANDARD_DATATYPE_TRAITS(uint8_t,  H5T_STD_U8LE)
STANDARD_DATATYPE_TRAITS(uint16_t,  H5T_STD_U16LE)
STANDARD_DATATYPE_TRAITS(uint32_t, H5T_STD_U32LE)
STANDARD_DATATYPE_TRAITS(uint64_t, H5T_STD_U64LE)
STANDARD_DATATYPE_TRAITS(float,    H5T_IEEE_F32LE)
STANDARD_DATATYPE_TRAITS(double,   H5T_IEEE_F64LE)

#undef STANDARD_DATATYPE_TRAITS

}  // namespace hdf5
}  // namespace lue
