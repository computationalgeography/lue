#pragma once
// #include "lue/hdf5/configure.h"
#include <hdf5.h>
#include <string>
#include <type_traits>


namespace lue {
namespace hdf5 {

template<
    typename T,
    typename Enable=void>
struct NativeDatatypeTraits
{
};


#define NATIVE_DATATYPE_TRAITS(                      \
    type,                                            \
    type_id_)                                        \
template<>                                           \
struct NativeDatatypeTraits<type>                    \
{                                                    \
    static hid_t type_id() { return type_id_; }      \
    static std::string name() { return #type_id_; }  \
};


NATIVE_DATATYPE_TRAITS(int8_t, H5T_NATIVE_INT8)
NATIVE_DATATYPE_TRAITS(int16_t, H5T_NATIVE_INT16)
NATIVE_DATATYPE_TRAITS(int32_t, H5T_NATIVE_INT32)
NATIVE_DATATYPE_TRAITS(int64_t, H5T_NATIVE_INT64)
NATIVE_DATATYPE_TRAITS(uint8_t, H5T_NATIVE_UINT8)
NATIVE_DATATYPE_TRAITS(uint16_t, H5T_NATIVE_UINT16)
NATIVE_DATATYPE_TRAITS(uint32_t, H5T_NATIVE_UINT32)
NATIVE_DATATYPE_TRAITS(uint64_t, H5T_NATIVE_UINT64)
NATIVE_DATATYPE_TRAITS(float, H5T_NATIVE_FLOAT)
NATIVE_DATATYPE_TRAITS(double, H5T_NATIVE_DOUBLE)

#undef NATIVE_DATATYPE_TRAITS


// If hsize_t is typedef-ed as uint64_t (macOS), we musn't ѕpecialize the
// template for hsize_t.
template<
    typename T>
struct NativeDatatypeTraits<
    T,
    typename std::enable_if<(
        std::is_same<T, hsize_t>::value &&
        !std::is_same<hsize_t, uint64_t>::value)>::type
>
{
    static hid_t type_id() { return H5T_NATIVE_HSIZE; }
    static std::string name() { return "H5T_NATIVE_HSIZE"; }
};


template<
    typename T,
    typename Enable=void>
struct StandardDatatypeTraits
{
};


#define STANDARD_DATATYPE_TRAITS(                    \
    type,                                            \
    type_id_)                                        \
template<>                                           \
struct StandardDatatypeTraits<type>                  \
{                                                    \
    static hid_t type_id() { return type_id_; }      \
    static std::string name() { return #type_id_; }  \
};


STANDARD_DATATYPE_TRAITS(int8_t, H5T_STD_I8LE)
STANDARD_DATATYPE_TRAITS(int16_t, H5T_STD_I16LE)
STANDARD_DATATYPE_TRAITS(int32_t, H5T_STD_I32LE)
STANDARD_DATATYPE_TRAITS(int64_t, H5T_STD_I64LE)
STANDARD_DATATYPE_TRAITS(uint8_t, H5T_STD_U8LE)
STANDARD_DATATYPE_TRAITS(uint16_t, H5T_STD_U16LE)
STANDARD_DATATYPE_TRAITS(uint32_t, H5T_STD_U32LE)
STANDARD_DATATYPE_TRAITS(uint64_t, H5T_STD_U64LE)
STANDARD_DATATYPE_TRAITS(float, H5T_IEEE_F32LE)
STANDARD_DATATYPE_TRAITS(double, H5T_IEEE_F64LE)

#undef STANDARD_DATATYPE_TRAITS


// In case hsize_t is not defined as being uint64_t (non-macOS), we must
// specialize the template for hsize_t.
template<
    typename T>
struct StandardDatatypeTraits<
    T,
    typename std::enable_if<(
        std::is_same<T, hsize_t>::value &&
        !std::is_same<hsize_t, uint64_t>::value)>::type
>
    : public StandardDatatypeTraits<uint64_t>
{
    // hsize_t is not defined as being uint64_t
    // Here we assume that hsize_t
    // - is of a type for which we haven't overloaded the template yet
    // - is typedef-ed as unsigned long long int
    // - is treated by the compiler the same as uint64_t
    //
    // If this is not the case, we want to know about it.

    static_assert(sizeof(hsize_t) == 8,
        "expecting size of hsize_t to be 8 bytes");
    static_assert(std::is_unsigned<hsize_t>::value,
        "expecting hsize_t to be unsigned");
    static_assert(std::is_same<hsize_t, unsigned long long int>::value,
        "expecting hsize_t to be unsigned long long int");
};

}  // namespace hdf5
}  // namespace lue
