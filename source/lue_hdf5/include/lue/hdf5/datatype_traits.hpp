#pragma once
#include <hdf5.h>
#include <string>
#include <type_traits>


namespace lue {
namespace hdf5 {

/*!
    @brief      Datatype traits of HDF5 native types corresponding to @a T

    This template is implemented for:
    - int8_t
    - int16_t
    - int32_t
    - int64_t
    - uint8_t
    - uint16_t
    - uint32_t
    - uint64_t
    - float
    - double
    - hsize_t
*/
template<
    typename T,
    typename Enable=void>
struct NativeDatatypeTraits
{

    /*!
        @brief      Return id of native HDF5 type
    */
    static hid_t   type_id             ();

    /*!
        @brief      Return name of native HDF5 type
    */
    static std::string name            ();

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


/// @cond INCLUDE_DETAILS

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

/// @endcond


/*!
    @brief      Datatype traits of HDF5 standard types corresponding to @a T

    This template is implemented for:
    - int8_t
    - int16_t
    - int32_t
    - int64_t
    - uint8_t
    - uint16_t
    - uint32_t
    - uint64_t
    - float
    - double
    - hsize_t
*/
template<
    typename T,
    typename Enable=void>
struct StandardDatatypeTraits
{

    /*!
        @brief      Return id of standard HDF5 type
    */
    static hid_t   type_id             ();

    /*!
        @brief      Return name of standard HDF5 type
    */
    static std::string name            ();

    /*!
        @brief      Return number of bytes occupied by the standard HDF5 type
    */
    static std::size_t nr_bytes        ();

};


#define STANDARD_DATATYPE_TRAITS(                        \
    type,                                                \
    type_id_,                                            \
    nr_bytes_)                                           \
template<>                                               \
struct StandardDatatypeTraits<type>                      \
{                                                        \
    static hid_t type_id() { return type_id_; }          \
    static std::string name() { return #type_id_; }      \
    static std::size_t nr_bytes() { return nr_bytes_; }  \
};


STANDARD_DATATYPE_TRAITS(int8_t, H5T_STD_I8LE, 1)
STANDARD_DATATYPE_TRAITS(int16_t, H5T_STD_I16LE, 2)
STANDARD_DATATYPE_TRAITS(int32_t, H5T_STD_I32LE, 4)
STANDARD_DATATYPE_TRAITS(int64_t, H5T_STD_I64LE, 8)
STANDARD_DATATYPE_TRAITS(uint8_t, H5T_STD_U8LE, 1)
STANDARD_DATATYPE_TRAITS(uint16_t, H5T_STD_U16LE, 2)
STANDARD_DATATYPE_TRAITS(uint32_t, H5T_STD_U32LE, 4)
STANDARD_DATATYPE_TRAITS(uint64_t, H5T_STD_U64LE, 8)
STANDARD_DATATYPE_TRAITS(float, H5T_IEEE_F32LE, 4)
STANDARD_DATATYPE_TRAITS(double, H5T_IEEE_F64LE, 8)

#undef STANDARD_DATATYPE_TRAITS


/// @cond INCLUDE_DETAILS

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

/// @endcond

}  // namespace hdf5
}  // namespace lue
