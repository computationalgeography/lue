#pragma once
#include <cstdint>
#include <hdf5.h>
#include <string>
#include <type_traits>


namespace lue::hdf5 {

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
    template<typename T, typename Enable = void>
    struct NativeDatatypeTraits
    {

            /*!
                @brief      Return id of native HDF5 type
            */
            static auto type_id() -> ::hid_t;

            /*!
                @brief      Return name of native HDF5 type
            */
            static auto name() -> std::string;
    };


#define NATIVE_DATATYPE_TRAITS(type, type_id_)                                                               \
    template<>                                                                                               \
    struct NativeDatatypeTraits<type>                                                                        \
    {                                                                                                        \
            static auto type_id() -> hid_t                                                                   \
            {                                                                                                \
                return type_id_;                                                                             \
            }                                                                                                \
                                                                                                             \
            static auto name() -> std::string                                                                \
            {                                                                                                \
                return #type_id_;                                                                            \
            }                                                                                                \
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
    template<typename T>
    struct NativeDatatypeTraits<
        T,
        std::enable_if_t<(std::is_same_v<T, hsize_t> && !std::is_same_v<hsize_t, std::uint64_t>)>>
    {
            static auto type_id() -> ::hid_t
            {
                return H5T_NATIVE_HSIZE;
            }

            static auto name() -> std::string
            {
                return "H5T_NATIVE_HSIZE";
            }
    };

    /// @endcond


    template<>
    struct NativeDatatypeTraits<std::string>
    {
            static auto name() -> std::string
            {
                return "string";
            }
    };


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
    template<typename T, typename Enable = void>
    struct StandardDatatypeTraits
    {

            /*!
                @brief      Return id of standard HDF5 type
            */
            static auto type_id() -> ::hid_t;

            /*!
                @brief      Return name of standard HDF5 type
            */
            static auto name() -> std::string;

            /*!
                @brief      Return number of bytes occupied by the standard HDF5 type
            */
            static auto nr_bytes() -> std::size_t;
    };


#define STANDARD_DATATYPE_TRAITS(type, type_id_, nr_bytes_)                                                  \
    template<>                                                                                               \
    struct StandardDatatypeTraits<type>                                                                      \
    {                                                                                                        \
            static auto type_id() -> hid_t                                                                   \
            {                                                                                                \
                return type_id_;                                                                             \
            }                                                                                                \
                                                                                                             \
            static auto name() -> std::string                                                                \
            {                                                                                                \
                return #type_id_;                                                                            \
            }                                                                                                \
                                                                                                             \
            static auto nr_bytes() -> std::size_t                                                            \
            {                                                                                                \
                return nr_bytes_;                                                                            \
            }                                                                                                \
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
    template<typename T>
    struct StandardDatatypeTraits<
        T,
        std::enable_if_t<(std::is_same_v<T, hsize_t> && !std::is_same_v<hsize_t, std::uint64_t>)>>:
        public StandardDatatypeTraits<std::uint64_t>
    {
            // hsize_t is not defined as being uint64_t
            // Here we assume that hsize_t
            // - is of a type for which we haven't overloaded the template yet
            // - is treated by the compiler the same as uint64_t
            //
            // If this is not the case, we want to know about it.

            // NOLINTNEXTLINE(readability-magic-numbers)
            static_assert(sizeof(hsize_t) == 8, "expecting size of hsize_t to be 8 bytes");
            static_assert(std::is_unsigned_v<hsize_t>, "expecting hsize_t to be unsigned");
    };

    /// @endcond


    template<>
    struct StandardDatatypeTraits<std::string>
    {
            static auto name() -> std::string
            {
                return "string";
            }
    };

}  // namespace lue::hdf5
