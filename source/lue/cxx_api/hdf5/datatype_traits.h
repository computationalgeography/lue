#pragma once
#include <string>


namespace lue {

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

NATIVE_DATATYPE_TRAITS(int32_t, H5T_NATIVE_INT32)
NATIVE_DATATYPE_TRAITS(int64_t, H5T_NATIVE_INT64)
NATIVE_DATATYPE_TRAITS(uint32_t, H5T_NATIVE_UINT32)
NATIVE_DATATYPE_TRAITS(uint64_t, H5T_NATIVE_UINT64)
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

STANDARD_DATATYPE_TRAITS(int32_t,  H5T_STD_I32LE)
STANDARD_DATATYPE_TRAITS(int64_t,  H5T_STD_I64LE)
STANDARD_DATATYPE_TRAITS(uint32_t, H5T_STD_U32LE)
STANDARD_DATATYPE_TRAITS(uint64_t, H5T_STD_U64LE)
STANDARD_DATATYPE_TRAITS(float,    H5T_IEEE_F32LE)
STANDARD_DATATYPE_TRAITS(double,   H5T_IEEE_F64LE)

#undef STANDARD_DATATYPE_TRAITS

}  // namespace lue
