#pragma once
#include <string>


namespace lue {

template<
    typename T>
struct MemoryDatatypeTraits
{
};

#define MEMORY_DATATYPE_TRAITS(  \
        type,                    \
        type_id_)                \
template<>                       \
struct MemoryDatatypeTraits<type>      \
{                                             \
    static hid_t type_id() { return type_id_; }  \
    static std::string name() { return #type_id_; } \
};

MEMORY_DATATYPE_TRAITS(int32_t, H5T_NATIVE_INT32)
MEMORY_DATATYPE_TRAITS(int64_t, H5T_NATIVE_INT64)
MEMORY_DATATYPE_TRAITS(uint32_t, H5T_NATIVE_UINT32)
MEMORY_DATATYPE_TRAITS(uint64_t, H5T_NATIVE_UINT64)
MEMORY_DATATYPE_TRAITS(float, H5T_NATIVE_FLOAT)
MEMORY_DATATYPE_TRAITS(double, H5T_NATIVE_DOUBLE)

#undef MEMORY_DATATYPE_TRAITS

}  // namespace lue
