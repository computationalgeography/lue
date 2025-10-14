#pragma once
#include "lue/hdf5/identifier.hpp"
#include <vector>


namespace lue {
    namespace hdf5 {

        /*!
            @brief      Class representing an HDF5 datatype
            @sa         [HDF5 predefined
           datatypes](https://support.hdfgroup.org/HDF5/doc/RM/PredefDTypes.html)
        */
        class LUE_HDF5_EXPORT Datatype
        {

            public:

                Datatype() = default;

                explicit Datatype(hid_t id);

                explicit Datatype(Identifier&& id);

                Datatype(Datatype const&) = default;

                Datatype(Datatype&&) = default;

                ~Datatype() = default;

                Datatype& operator=(Datatype const&) = default;

                Datatype& operator=(Datatype&&) = default;

                bool operator==(Datatype const& other) const;

                bool operator!=(Datatype const& other) const;

                Identifier const& id() const;

                ::H5T_class_t class_() const;

                void set_size(std::size_t nr_bytes);

                std::size_t size() const;

                ::H5T_cset_t encoding() const;

                bool is_standard() const;

                bool is_native() const;

                bool is_string() const;

                void insert(std::string const& name, std::size_t offset, Datatype const& datatype);

                // class Hash
                // {

                // public:

                //     std::size_t operator()(
                //         Datatype const& datatype) const noexcept
                //     {
                //         return Identifier::Hash{}(datatype._id);
                //     }

                // };

            private:

                //! Identifier of the datatype
                Identifier _id;
        };


        static auto const compare_datatypes = [](hdf5::Datatype const& lhs, hdf5::Datatype const& rhs) -> bool
        {
            // How to determine whether some data type is less than another one?
            // Potentially, datatype with different addresses represent the
            // same logical type (e.g. when a datatype is copied with H5Tcopy).

            // For now, assume this does not happen. Also, datatype equality
            // is tested using H5Tequal, which 'determines whether two
            // datatype identifiers refer to the same datatype'. Let's use
            // the same semantics.
            // return lhs.id().info().addr() < rhs.id().info().addr();

            // FIXME The same datatype can have different IDs pointing to it.
            //     Equality is the only thing we can trust.
            return lhs != rhs && lhs.id() < rhs.id();
        };

        using CompareDatatypes = decltype(compare_datatypes);


        LUE_HDF5_EXPORT std::vector<unsigned char> encode_datatype(Datatype const& datatype);

        LUE_HDF5_EXPORT Datatype decode_datatype(std::vector<unsigned char> const& buffer);

        LUE_HDF5_EXPORT std::string native_datatype_as_string(Datatype const& datatype);

        LUE_HDF5_EXPORT std::string standard_datatype_as_string(Datatype const& datatype);

        LUE_HDF5_EXPORT Datatype create_datatype(std::size_t nr_bytes);

        LUE_HDF5_EXPORT Datatype create_datatype(hid_t type_id, std::size_t nr_bytes);

        LUE_HDF5_EXPORT Datatype create_string_datatype();

        LUE_HDF5_EXPORT Datatype create_compound_datatype(std::size_t nr_bytes);

        LUE_HDF5_EXPORT Datatype memory_datatype(Datatype const& file_datatype);

        LUE_HDF5_EXPORT Datatype file_datatype(Datatype const& memory_datatype);


        static Datatype const native_uint8{H5T_NATIVE_UINT8};
        static Datatype const native_uint16{H5T_NATIVE_UINT16};
        static Datatype const native_uint32{H5T_NATIVE_UINT32};
        static Datatype const native_uint64{H5T_NATIVE_UINT64};
        static Datatype const native_int8{H5T_NATIVE_INT8};
        static Datatype const native_int16{H5T_NATIVE_INT16};
        static Datatype const native_int32{H5T_NATIVE_INT32};
        static Datatype const native_int64{H5T_NATIVE_INT64};
        static Datatype const native_float32{H5T_NATIVE_FLOAT};
        static Datatype const native_float64{H5T_NATIVE_DOUBLE};

        static Datatype const std_uint8_le{H5T_STD_U8LE};
        static Datatype const std_uint16_le{H5T_STD_U16LE};
        static Datatype const std_uint32_le{H5T_STD_U32LE};
        static Datatype const std_uint64_le{H5T_STD_U64LE};
        static Datatype const std_int8_le{H5T_STD_I8LE};
        static Datatype const std_int16_le{H5T_STD_I16LE};
        static Datatype const std_int32_le{H5T_STD_I32LE};
        static Datatype const std_int64_le{H5T_STD_I64LE};
        static Datatype const ieee_float32_le{H5T_IEEE_F32LE};
        static Datatype const ieee_float64_le{H5T_IEEE_F64LE};


        static std::vector<Datatype> const native_unsigned_integrals{
            native_uint8, native_uint16, native_uint32, native_uint64};


        static std::vector<Datatype> const native_signed_integrals{
            native_int8, native_int16, native_int32, native_int64};


        static std::vector<Datatype> const native_floating_points{native_float32, native_float64};


        static std::vector<Datatype> const std_unsigned_integrals_le{
            std_uint8_le, std_uint16_le, std_uint32_le, std_uint64_le};


        static std::vector<Datatype> const std_signed_integrals_le{
            std_int8_le, std_int16_le, std_int32_le, std_int64_le};


        static std::vector<Datatype> const ieee_floating_points_le{ieee_float32_le, ieee_float64_le};


        LUE_HDF5_EXPORT bool is_native_unsigned_integral(Datatype const& datatype);

        LUE_HDF5_EXPORT bool is_native_signed_integral(Datatype const& datatype);

        LUE_HDF5_EXPORT bool is_native_floating_point(Datatype const& datatype);

        LUE_HDF5_EXPORT bool is_std_unsigned_integral_le(Datatype const& datatype);

        LUE_HDF5_EXPORT bool is_std_signed_integral_le(Datatype const& datatype);

        LUE_HDF5_EXPORT bool is_ieee_floating_point_le(Datatype const& datatype);

        template<typename T>
        Datatype native_datatype();

        template<typename T>
        Datatype std_datatype_le();

        template<typename T>
        Datatype ieee_datatype_le();

        template<typename T>
        Datatype std_datatype();

    }  // namespace hdf5
}  // namespace lue
