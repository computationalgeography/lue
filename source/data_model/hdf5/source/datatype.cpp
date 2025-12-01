#include "lue/hdf5/datatype.hpp"
#include "lue/hdf5/datatype_traits.hpp"
#include <boost/predef/other/endian.h>
#include <algorithm>
#include <format>
#include <optional>


namespace lue::hdf5 {
    namespace {

        auto datatypes_are_equal(hid_t const type_id1, hid_t const type_id2) -> bool
        {
            auto const result = H5Tequal(type_id1, type_id2);
            assert(result >= 0);

            return result > 0;
        }

    }  // Anonymous namespace


    auto native_datatype_as_string(Datatype const& datatype) -> std::string
    {
        assert(datatype.is_native() || datatype.is_string());

        std::string result;

        if (datatype == native_float32)
        {
            result = NativeDatatypeTraits<float>::name();
        }
        else if (datatype == native_float64)
        {
            result = NativeDatatypeTraits<double>::name();
        }
        else if (datatype == native_uint8)
        {
            result = NativeDatatypeTraits<uint8_t>::name();
        }
        else if (datatype == native_uint16)
        {
            result = NativeDatatypeTraits<uint16_t>::name();
        }
        else if (datatype == native_uint32)
        {
            result = NativeDatatypeTraits<uint32_t>::name();
        }
        else if (datatype == native_uint64)
        {
            result = NativeDatatypeTraits<uint64_t>::name();
        }
        else if (datatype == native_int8)
        {
            result = NativeDatatypeTraits<int8_t>::name();
        }
        else if (datatype == native_int16)
        {
            result = NativeDatatypeTraits<int16_t>::name();
        }
        else if (datatype == native_int32)
        {
            result = NativeDatatypeTraits<int32_t>::name();
        }
        else if (datatype == native_int64)
        {
            result = NativeDatatypeTraits<int64_t>::name();
        }
        else if (datatype.is_string())
        {
            result = NativeDatatypeTraits<std::string>::name();
        }

        assert(!result.empty());

        return result;
    }


    auto standard_datatype_as_string(Datatype const& datatype) -> std::string
    {
        assert(datatype.is_standard() || datatype.is_string());

        std::string result;

        if (datatype == std_int8_le)
        {
            result = StandardDatatypeTraits<int8_t>::name();
        }
        else if (datatype == std_int16_le)
        {
            result = StandardDatatypeTraits<int16_t>::name();
        }
        else if (datatype == std_int32_le)
        {
            result = StandardDatatypeTraits<int32_t>::name();
        }
        else if (datatype == std_int64_le)
        {
            result = StandardDatatypeTraits<int64_t>::name();
        }
        else if (datatype == std_uint8_le)
        {
            result = StandardDatatypeTraits<uint8_t>::name();
        }
        else if (datatype == std_uint16_le)
        {
            result = StandardDatatypeTraits<uint16_t>::name();
        }
        else if (datatype == std_uint32_le)
        {
            result = StandardDatatypeTraits<uint32_t>::name();
        }
        else if (datatype == std_uint64_le)
        {
            result = StandardDatatypeTraits<uint64_t>::name();
        }
        else if (datatype == ieee_float32_le)
        {
            result = StandardDatatypeTraits<float>::name();
        }
        else if (datatype == ieee_float64_le)
        {
            result = StandardDatatypeTraits<double>::name();
        }
        else if (datatype.is_string())
        {
            result = StandardDatatypeTraits<std::string>::name();
        }

        assert(!result.empty());

        return result;
    }


    /*!
        The data type is not closed upon destruction
    */
    Datatype::Datatype(hid_t const id):

        Datatype{Identifier{id, []([[maybe_unused]] hid_t const id) -> int { return 0; }}}

    {
    }


    Datatype::Datatype(Identifier&& id):

        _id(std::move(id))

    {
        assert(_id.type() == H5I_DATATYPE);
    }


    auto Datatype::id() const -> Identifier const&
    {
        return _id;
    }


    auto Datatype::class_() const -> H5T_class_t
    {
        return H5Tget_class(_id);
    }


    void Datatype::set_size(std::size_t const nr_bytes)
    {
        herr_t const status{H5Tset_size(_id, nr_bytes)};

        if (status < 0)
        {
            throw std::runtime_error("Cannot set size");
        }
    }


    auto Datatype::size() const -> std::size_t
    {
        return H5Tget_size(_id);
    }


    /*!
        @brief      Retrieve the encoding of a string datatype
        @exception  std::runtime_error In case the encoding cannot be
                    retrieved (no string datatype?)
    */
    auto Datatype::encoding() const -> H5T_cset_t
    {
        H5T_cset_t const encoding{H5Tget_cset(_id)};

        if (encoding == H5T_CSET_ERROR)
        {
            throw std::runtime_error("Cannot retrieve encoding");
        }

        return encoding;
    }


    auto Datatype::is_standard() const -> bool
    {
#if !defined(BOOST_ENDIAN_LITTLE_BYTE)
        // We assume little endian platforms in tests. This can be changed
        // when necessary. In that case, first check whether the tests for
        // standard data types are really necessary. Ideally, let HDF
        // figure it all out.
        BOOST_STATIC_ASSERT(false);
#endif

        return is_std_unsigned_integral_le(*this) || is_std_signed_integral_le(*this) ||
               is_ieee_floating_point_le(*this);
    }


    auto Datatype::is_native() const -> bool
    {
        return is_native_unsigned_integral(*this) || is_native_signed_integral(*this) ||
               is_native_floating_point(*this);
    }


    auto Datatype::is_string() const -> bool
    {
        return class_() == H5T_STRING;
    }


    void Datatype::insert(std::string const& name, std::size_t offset, Datatype const& datatype)
    {
        herr_t const status{H5Tinsert(_id, name.c_str(), offset, datatype.id())};

        if (status < 0)
        {
            throw std::runtime_error("Cannot insert member to compound datatype");
        }
    }


    auto Datatype::operator==(Datatype const& other) const -> bool
    {
        return datatypes_are_equal(_id, other._id);
    }


    auto Datatype::operator!=(Datatype const& other) const -> bool
    {
        return !datatypes_are_equal(_id, other._id);
    }


    auto encode_datatype(Datatype const& datatype) -> std::vector<unsigned char>
    {
        // Determine size of buffer.
        std::size_t nr_bytes{};
        herr_t status{H5Tencode(datatype.id(), nullptr, &nr_bytes)};

        if (status < 0)
        {
            throw std::runtime_error("Cannot encode data type");
        }

        // Encode data type.
        std::vector<unsigned char> buffer(nr_bytes);

        status = H5Tencode(datatype.id(), buffer.data(), &nr_bytes);

        if (status < 0)
        {
            throw std::runtime_error("Cannot encode data type");
        }

        return buffer;
    }


    auto decode_datatype(std::vector<unsigned char> const& buffer) -> Datatype
    {
        hid_t const id{H5Tdecode(buffer.data())};

        if (id < 0)
        {
            throw std::runtime_error("Cannot decode data type");
        }

        return Datatype{Identifier{id, H5Tclose}};
    }


    namespace {

        auto create_datatype_(H5T_class_t const class_, std::size_t const nr_bytes) -> Datatype
        {
            Identifier id{H5Tcreate(class_, nr_bytes), H5Tclose};

            if (!id.is_valid())
            {
                throw std::runtime_error("Cannot create compound data type");
            }

            return Datatype{std::move(id)};
        }


        auto copy_datatype(::hid_t const datatype_id) -> Datatype
        {
            Identifier id{H5Tcopy(datatype_id), H5Tclose};

            if (!id.is_valid())
            {
                throw std::runtime_error("Cannot copy data type");
            }

            return Datatype{std::move(id)};
        }

    }  // Anonymous namespace


    /*!
        @brief      Create a datatype for a UTF8-encoded string
        @param      nr_bytes Total size of the datatype
        @exception  std::runtime_error In case the datatype cannot be created
        @exception  std::runtime_error In case the character set cannot be set
    */
    auto create_datatype(std::size_t const nr_bytes) -> Datatype
    {
        Datatype datatype{create_datatype(H5T_C_S1, nr_bytes)};
        herr_t const status = H5Tset_cset(datatype.id(), H5T_CSET_UTF8);

        if (status < 0)
        {
            throw std::runtime_error("Cannot set character set");
        }

        return datatype;
    }


    /*!
        @brief      Create a datatype for a sequence of elements
        @param      nr_bytes Total size of the datatype
        @exception  std::runtime_error In case the datatype cannot be created
    */
    Datatype create_datatype(::hid_t const type_id, std::size_t const nr_bytes)
    {
        Identifier id{H5Tcopy(type_id), H5Tclose};

        if (!id.is_valid())
        {
            throw std::runtime_error("Cannot copy type-id");
        }

        Datatype datatype{std::move(id)};
        datatype.set_size(nr_bytes);

        return datatype;
    }


    /*!
        @brief      Create a datatype for variable length UTF8 encoded strings

        The string datatype used is H5T_C_S1, not H5T_FORTRAN_S1.

        Variable-length strings will always be NULL-terminated, so the buffer
        to hold such a string must be one byte larger than the string itself
        to accommodate the NULL terminator.
    */
    auto create_string_datatype() -> Datatype
    {
        Datatype datatype{copy_datatype(H5T_C_S1)};
        herr_t const status{H5Tset_cset(datatype.id(), H5T_CSET_UTF8)};

        if (status < 0)
        {
            throw std::runtime_error("Cannot set character set");
        }

        datatype.set_size(H5T_VARIABLE);

        return datatype;
    }


    auto create_compound_datatype(std::size_t const nr_bytes) -> Datatype
    {
        Datatype datatype{create_datatype_(H5T_COMPOUND, nr_bytes)};

        datatype.set_size(nr_bytes);

        assert(datatype.id().is_valid());

        return datatype;
    }


    auto memory_datatype(Datatype const& file_datatype) -> Datatype
    {
        assert(file_datatype.is_standard() || file_datatype.is_string());

        std::optional<Datatype> result;

        if (file_datatype == std_uint8_le)
        {
            result = native_uint8;
        }
        else if (file_datatype == std_uint16_le)
        {
            result = native_uint16;
        }
        else if (file_datatype == std_uint32_le)
        {
            result = native_uint32;
        }
        else if (file_datatype == std_uint64_le)
        {
            result = native_uint64;
        }
        else if (file_datatype == std_int8_le)
        {
            result = native_int8;
        }
        else if (file_datatype == std_int16_le)
        {
            result = native_int16;
        }
        else if (file_datatype == std_int32_le)
        {
            result = native_int32;
        }
        else if (file_datatype == std_int64_le)
        {
            result = native_int64;
        }
        else if (file_datatype == ieee_float32_le)
        {
            result = native_float32;
        }
        else if (file_datatype == ieee_float64_le)
        {
            result = native_float64;
        }
        else if (file_datatype.is_string())
        {
            result = file_datatype;
        }

        if (!result)
        {
            throw std::runtime_error(
                std::format(
                    "No memory datatype for file datatype ({})", standard_datatype_as_string(file_datatype)));
        }

        return *result;

        // Hashing based on object ID does not work. The same type can have different IDs...
        // static std::unordered_map<Datatype, Datatype, Datatype::Hash> const map{
        //         {std_uint8_le, native_uint8},
        //         {std_uint16_le, native_uint16},
        //         {std_uint32_le, native_uint32},
        //         {std_uint64_le, native_uint64},
        //         {std_int8_le, native_int8},
        //         {std_int16_le, native_int16},
        //         {std_int32_le, native_int32},
        //         {std_int64_le, native_int64},
        //         {ieee_float32_le, native_float32},
        //         {ieee_float64_le, native_float64},
        //     };

        // Datatype result{};

        // if(auto const it{map.find(file_datatype)}; it != map.end())
        // {
        //     result = it->second;
        // }
        // else if(file_datatype.is_string())
        // {
        //     result = file_datatype;
        // }
        // else
        // {
        //     throw std::runtime_error(std::format(
        //         "No memory datatype for file datatype ({})",
        //         standard_datatype_as_string(file_datatype))
        //     );
        // }

        // return result;
    }


    auto file_datatype(Datatype const& memory_datatype) -> Datatype
    {
        assert(memory_datatype.is_native() || memory_datatype.is_string());

        std::optional<Datatype> result;

        if (memory_datatype == native_uint8)
        {
            result = std_uint8_le;
        }
        else if (memory_datatype == native_uint16)
        {
            result = std_uint16_le;
        }
        else if (memory_datatype == native_uint32)
        {
            result = std_uint32_le;
        }
        else if (memory_datatype == native_uint64)
        {
            result = std_uint64_le;
        }
        else if (memory_datatype == native_int8)
        {
            result = std_int8_le;
        }
        else if (memory_datatype == native_int16)
        {
            result = std_int16_le;
        }
        else if (memory_datatype == native_int32)
        {
            result = std_int32_le;
        }
        else if (memory_datatype == native_int64)
        {
            result = std_int64_le;
        }
        else if (memory_datatype == native_float32)
        {
            result = ieee_float32_le;
        }
        else if (memory_datatype == native_float64)
        {
            result = ieee_float64_le;
        }
        else if (memory_datatype.is_string())
        {
            result = memory_datatype;
        }

        if (!result)
        {
            throw std::runtime_error(
                std::format(
                    "No file datatype for memory datatype ({})", native_datatype_as_string(memory_datatype)));
        }

        return *result;

        // Hashing based on object ID does not work. The same type can have different IDs...
        // static std::unordered_map<Datatype, Datatype, Datatype::Hash> const map{
        //         {native_uint8, std_uint8_le},
        //         {native_uint16, std_uint16_le},
        //         {native_uint32, std_uint32_le},
        //         {native_uint64, std_uint64_le},
        //         {native_int8, std_int8_le},
        //         {native_int16, std_int16_le},
        //         {native_int32, std_int32_le},
        //         {native_int64, std_int64_le},
        //         {native_float32, ieee_float32_le},
        //         {native_float64, ieee_float64_le},
        //     };

        // Datatype result{};

        // if(auto const it{map.find(memory_datatype)}; it != map.end())
        // {
        //     result = it->second;
        // }
        // else if(memory_datatype.is_string())
        // {
        //     result = memory_datatype;
        // }
        // else
        // {
        //     throw std::runtime_error(std::format(
        //         "No file datatype for memory datatype ({})",
        //         native_datatype_as_string(memory_datatype))
        //     );
        // }

        // return result;
    }


    auto is_native_unsigned_integral(Datatype const& datatype) -> bool
    {
        return std::ranges::find(native_unsigned_integrals, datatype) != native_unsigned_integrals.end();
    }


    auto is_native_signed_integral(Datatype const& datatype) -> bool
    {
        return std::ranges::find(native_signed_integrals, datatype) != native_signed_integrals.end();
    }


    auto is_native_floating_point(Datatype const& datatype) -> bool
    {
        return std::ranges::find(native_floating_points, datatype) != native_floating_points.end();
    }


    auto is_std_unsigned_integral_le(Datatype const& datatype) -> bool
    {
        return std::ranges::find(std_unsigned_integrals_le, datatype) != std_unsigned_integrals_le.end();
    }


    auto is_std_signed_integral_le(Datatype const& datatype) -> bool
    {
        return std::ranges::find(std_signed_integrals_le, datatype) != std_signed_integrals_le.end();
    }


    auto is_ieee_floating_point_le(Datatype const& datatype) -> bool
    {
        return std::ranges::find(ieee_floating_points_le, datatype) != ieee_floating_points_le.end();
    }


#define NATIVE_DATATYPE(type, datatype)                                                                      \
    template<>                                                                                               \
    LUE_HDF5_EXPORT auto native_datatype<type>() -> Datatype                                                 \
    {                                                                                                        \
        return native_##datatype;                                                                            \
    }

    NATIVE_DATATYPE(std::uint8_t, uint8)
    NATIVE_DATATYPE(std::uint16_t, uint16)
    NATIVE_DATATYPE(std::uint32_t, uint32)
    NATIVE_DATATYPE(std::uint64_t, uint64)
    NATIVE_DATATYPE(std::int8_t, int8)
    NATIVE_DATATYPE(std::int16_t, int16)
    NATIVE_DATATYPE(std::int32_t, int32)
    NATIVE_DATATYPE(std::int64_t, int64)
    NATIVE_DATATYPE(float, float32)
    NATIVE_DATATYPE(double, float64)

#undef NATIVE_DATATYPE

    template<>
    LUE_HDF5_EXPORT auto native_datatype<std::string>() -> Datatype
    {
        static auto const datatype = create_string_datatype();
        return datatype;
    }


#define STD_DATATYPE_LE(type, datatype)                                                                      \
    template<>                                                                                               \
    LUE_HDF5_EXPORT auto std_datatype_le<type>() -> Datatype                                                 \
    {                                                                                                        \
        return std_##datatype;                                                                               \
    }

    STD_DATATYPE_LE(std::uint8_t, uint8_le)
    STD_DATATYPE_LE(std::uint16_t, uint16_le)
    STD_DATATYPE_LE(std::uint32_t, uint32_le)
    STD_DATATYPE_LE(std::uint64_t, uint64_le)
    STD_DATATYPE_LE(std::int8_t, int8_le)
    STD_DATATYPE_LE(std::int16_t, int16_le)
    STD_DATATYPE_LE(std::int32_t, int32_le)
    STD_DATATYPE_LE(std::int64_t, int64_le)

#undef STD_DATATYPE_LE


#define IEEE_DATATYPE_LE(type, datatype)                                                                     \
    template<>                                                                                               \
    LUE_HDF5_EXPORT auto ieee_datatype_le<type>() -> Datatype                                                \
    {                                                                                                        \
        return ieee_##datatype;                                                                              \
    }

    IEEE_DATATYPE_LE(float, float32_le)
    IEEE_DATATYPE_LE(double, float64_le)

#undef IEEE_DATATYPE_LE


    template<>
    LUE_HDF5_EXPORT auto std_datatype<std::string>() -> Datatype
    {
        static auto const datatype = create_string_datatype();
        return datatype;
    }

}  // namespace lue::hdf5
