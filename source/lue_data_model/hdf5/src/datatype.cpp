#include "lue/hdf5/datatype.hpp"
#include "lue/hdf5/datatype_traits.hpp"
#include <boost/detail/endian.hpp>
#include <algorithm>
#include <cassert>
#include <set>


namespace lue {
namespace hdf5 {
namespace {

bool datatypes_are_equal(
    hid_t const type_id1,
    hid_t const type_id2)
{
    auto const result = ::H5Tequal(type_id1, type_id2);
    assert(result >= 0);

    return result > 0;
}

}  // Anonymous namespace


std::string native_datatype_as_string(
    Datatype const& datatype)
{
    assert(datatype.is_native());

    std::string result;

    if(datatype == native_float32) {
        result = NativeDatatypeTraits<float>::name();
    }
    else if(datatype == native_float64) {
        result = NativeDatatypeTraits<double>::name();
    }
    else if(datatype == native_uint8) {
        result = NativeDatatypeTraits<uint8_t>::name();
    }
    else if(datatype == native_uint16) {
        result = NativeDatatypeTraits<uint16_t>::name();
    }
    else if(datatype == native_uint32) {
        result = NativeDatatypeTraits<uint32_t>::name();
    }
    else if(datatype == native_uint64) {
        result = NativeDatatypeTraits<uint64_t>::name();
    }
    else if(datatype == native_int8) {
        result = NativeDatatypeTraits<int8_t>::name();
    }
    else if(datatype == native_int16) {
        result = NativeDatatypeTraits<int16_t>::name();
    }
    else if(datatype == native_int32) {
        result = NativeDatatypeTraits<int32_t>::name();
    }
    else if(datatype == native_int64) {
        result = NativeDatatypeTraits<int64_t>::name();
    }

    assert(!result.empty());

    return result;
}


std::string standard_datatype_as_string(
    Datatype const& datatype)
{
    assert(datatype.is_standard());

    std::string result;

    if(datatype == std_int8_le) {
        result = StandardDatatypeTraits<int8_t>::name();
    }
    else if(datatypes_are_equal(datatype.id(), H5T_STD_I8BE)) {
        result = "H5T_STD_I8BE";
    }
    else if(datatype == std_int16_le) {
        result = StandardDatatypeTraits<int16_t>::name();
    }
    else if(datatypes_are_equal(datatype.id(), H5T_STD_I16BE)) {
        result = "H5T_STD_I16BE";
    }
    else if(datatype == std_int32_le) {
        result = StandardDatatypeTraits<int32_t>::name();
    }
    else if(datatypes_are_equal(datatype.id(), H5T_STD_I32BE)) {
        result = "H5T_STD_I32BE";
    }
    else if(datatype == std_int64_le) {
        result = StandardDatatypeTraits<int64_t>::name();
    }
    else if(datatypes_are_equal(datatype.id(), H5T_STD_I64BE)) {
        result = "H5T_STD_I64BE";
    }
    else if(datatype == std_uint8_le) {
        result = StandardDatatypeTraits<uint8_t>::name();
    }
    else if(datatypes_are_equal(datatype.id(), H5T_STD_U8BE)) {
        result = "H5T_STD_U8BE";
    }
    else if(datatype == std_uint16_le) {
        result = StandardDatatypeTraits<uint16_t>::name();
    }
    else if(datatypes_are_equal(datatype.id(), H5T_STD_U16BE)) {
        result = "H5T_STD_U16BE";
    }
    else if(datatype == std_uint32_le) {
        result = StandardDatatypeTraits<uint32_t>::name();
    }
    else if(datatypes_are_equal(datatype.id(), H5T_STD_U32BE)) {
        result = "H5T_STD_U32BE";
    }
    else if(datatype == std_uint64_le) {
        result = StandardDatatypeTraits<uint64_t>::name();
    }
    else if(datatypes_are_equal(datatype.id(), H5T_STD_U64BE)) {
        result = "H5T_STD_U64BE";
    }
    else if(datatype == ieee_float32_le) {
        result = StandardDatatypeTraits<float>::name();
    }
    else if(datatypes_are_equal(datatype.id(), H5T_IEEE_F32BE)) {
        result = "H5T_STD_F32BE";
    }
    else if(datatype == ieee_float64_le) {
        result = StandardDatatypeTraits<double>::name();
    }
    else if(datatypes_are_equal(datatype.id(), H5T_IEEE_F64BE)) {
        result = "H5T_STD_F64BE";
    }

    assert(!result.empty());

    return result;
}


/*!
    @brief      .
    @param      .
    @return     .
    @exception  .

    The data type is not closed upon destruction
*/
Datatype::Datatype(
    hid_t const id)

    : _id(id, [](hid_t){ return 0; })

{
}


Datatype::Datatype(
    Identifier&& id)

    : _id(std::forward<Identifier>(id))

{
}


Identifier const& Datatype::id() const
{
    return _id;
}


void Datatype::set_size(
    std::size_t const nr_bytes)
{
    auto status = ::H5Tset_size(_id, nr_bytes);

    if(status < 0) {
        throw std::runtime_error("Cannot set size");
    }
}


std::size_t Datatype::size() const
{
    return ::H5Tget_size(_id);
}


/*!
    @brief      Retrieve the encoding of a string datatype
    @exception  std::runtime_error In case the encoding cannot be
                retrieved (no string datatype?)
*/
::H5T_cset_t Datatype::encoding() const
{
    auto encoding = ::H5Tget_cset(_id);

    if(encoding == ::H5T_CSET_ERROR) {
        throw std::runtime_error("Cannot retrieve encoding");
    }

    return encoding;
}


bool Datatype::is_standard() const
{
#if !defined(BOOST_LITTLE_ENDIAN)
    // We assume little endian platforms in tests. This can be changed
    // when necessary. In that case, first check whether the tests for
    // standard data types are really necessary. Ideally, let HDF
    // figure it all out.
    BOOST_STATIC_ASSERT(false);
#endif

    return
        is_std_unsigned_integral_le(*this) ||
        is_std_signed_integral_le(*this) ||
        is_ieee_floating_point_le(*this);
}


bool Datatype::is_native() const
{
    return
        is_native_unsigned_integral(*this) ||
        is_native_signed_integral(*this) ||
        is_native_floating_point(*this);
}


void Datatype::insert(
    std::string const& name,
    std::size_t offset,
    Datatype const& datatype)
{
    auto status = ::H5Tinsert(_id, name.c_str(), offset, datatype.id());

    if(status < 0) {
        throw std::runtime_error("Cannot insert member to compound datatype");
    }
}


bool Datatype::operator==(
    Datatype const& other) const
{
    return datatypes_are_equal(_id, other._id);
}


bool Datatype::operator!=(
    Datatype const& other) const
{
    return !datatypes_are_equal(_id, other._id);
}


std::vector<unsigned char> encode_datatype(
    Datatype const& datatype)
{
    // Determine size of buffer.
    std::size_t nr_bytes;
    auto status = ::H5Tencode(datatype.id(), nullptr, &nr_bytes);

    if(status < 0) {
        throw std::runtime_error("Cannot encode data type");
    }


    // Encode data type.
    std::vector<unsigned char> buffer(nr_bytes);

    status = ::H5Tencode(datatype.id(), buffer.data(), &nr_bytes);

    if(status < 0) {
        throw std::runtime_error("Cannot encode data type");
    }

    return buffer;
}


Datatype decode_datatype(
    std::vector<unsigned char> const& buffer)
{
    auto id = ::H5Tdecode(buffer.data());

    if(id < 0) {
        throw std::runtime_error("Cannot decode data type");
    }

    return Datatype(Identifier(id, ::H5Tclose));
}


namespace {

Datatype create_datatype_(
    H5T_class_t const class_,
    std::size_t const nr_bytes)
{
    auto id = Identifier(::H5Tcreate(class_, nr_bytes), ::H5Tclose);

    if(!id.is_valid()) {
        throw std::runtime_error("Cannot create compound data type");
    }

    Datatype datatype{std::move(id)};

    return datatype;
}


Datatype copy_datatype(
    hid_t const datatype_id)
{
    auto id = Identifier(::H5Tcopy(datatype_id), ::H5Tclose);

    if(!id.is_valid()) {
        throw std::runtime_error("Cannot copy data type");
    }

    Datatype datatype{std::move(id)};

    return datatype;
}

}  // Anonymous namespace


/*!
    @brief      Create a datatype for a UTF8-encoded string
    @param      nr_bytes Total size of the datatype
    @exception  std::runtime_error In case the datatype cannot be created
    @exception  std::runtime_error In case the character set cannot be set
*/
Datatype create_datatype(
    std::size_t const nr_bytes)
{
    auto datatype = create_datatype(H5T_C_S1, nr_bytes);
    auto status = ::H5Tset_cset(datatype.id(), H5T_CSET_UTF8);

    if(status < 0) {
        throw std::runtime_error("Cannot set character set");
    }

    return datatype;
}


/*!
    @brief      Create a datatype for a sequence of elements
    @param      nr_bytes Total size of the datatype
    @exception  std::runtime_error In case the datatype cannot be created
*/
Datatype create_datatype(
    hid_t const type_id,
    std::size_t const nr_bytes)
{
    auto id = Identifier(::H5Tcopy(type_id), ::H5Tclose);

    if(!id.is_valid()) {
        throw std::runtime_error("Cannot copy type-id");
    }

    Datatype datatype(std::move(id));
    datatype.set_size(nr_bytes);

    return datatype;
}


Datatype create_string_datatype()
{
    auto datatype = copy_datatype(H5T_C_S1);
    auto status = ::H5Tset_cset(datatype.id(), H5T_CSET_UTF8);

    if(status < 0) {
        throw std::runtime_error("Cannot set character set");
    }

    datatype.set_size(H5T_VARIABLE);

    return datatype;
}


Datatype create_compound_datatype(
    std::size_t const nr_bytes)
{
    auto datatype = create_datatype_(H5T_COMPOUND, nr_bytes);

    datatype.set_size(nr_bytes);

    assert(datatype.id().is_valid());

    return datatype;
}


Datatype memory_datatype(
    Datatype const& file_datatype)
{
    assert(file_datatype.is_standard());

    hid_t type_id = -1;

    if(file_datatype == Datatype{H5T_STD_I8LE}) {
        type_id = H5T_NATIVE_INT8;
    }
    else if(file_datatype == Datatype{H5T_STD_I16LE}) {
        type_id = H5T_NATIVE_INT16;
    }
    else if(file_datatype == Datatype{H5T_STD_I32LE}) {
        type_id = H5T_NATIVE_INT32;
    }
    else if(file_datatype == Datatype{H5T_STD_I64LE}) {
        type_id = H5T_NATIVE_INT64;
    }
    else if(file_datatype == Datatype{H5T_STD_U8LE}) {
        type_id = H5T_NATIVE_UINT8;
    }
    else if(file_datatype == Datatype{H5T_STD_U16LE}) {
        type_id = H5T_NATIVE_UINT16;
    }
    else if(file_datatype == Datatype{H5T_STD_U32LE}) {
        type_id = H5T_NATIVE_UINT32;
    }
    else if(file_datatype == Datatype{H5T_STD_U64LE}) {
        type_id = H5T_NATIVE_UINT64;
    }
    else if(file_datatype == Datatype{H5T_IEEE_F32LE}) {
        type_id = H5T_NATIVE_FLOAT;
    }
    else if(file_datatype == Datatype{H5T_IEEE_F64LE}) {
        type_id = H5T_NATIVE_DOUBLE;
    }

    assert(type_id >= 0);

    return Datatype(type_id);
}


Datatype file_datatype(
    Datatype const& memory_datatype)
{
    assert(memory_datatype.is_native());

    hid_t type_id = -1;

    if(memory_datatype == Datatype{H5T_NATIVE_INT8}) {
        type_id = H5T_STD_I8LE;
    }
    else if(memory_datatype == Datatype{H5T_NATIVE_INT16}) {
        type_id = H5T_STD_I16LE;
    }
    else if(memory_datatype == Datatype{H5T_NATIVE_INT32}) {
        type_id = H5T_STD_I32LE;
    }
    else if(memory_datatype == Datatype{H5T_NATIVE_INT64}) {
        type_id = H5T_STD_I64LE;
    }
    else if(memory_datatype == Datatype{H5T_NATIVE_UINT8}) {
        type_id = H5T_STD_U8LE;
    }
    else if(memory_datatype == Datatype{H5T_NATIVE_UINT16}) {
        type_id = H5T_STD_U16LE;
    }
    else if(memory_datatype == Datatype{H5T_NATIVE_UINT32}) {
        type_id = H5T_STD_U32LE;
    }
    else if(memory_datatype == Datatype{H5T_NATIVE_UINT64}) {
        type_id = H5T_STD_U64LE;
    }
    else if(memory_datatype == Datatype{H5T_NATIVE_FLOAT}) {
        type_id = H5T_IEEE_F32LE;
    }
    else if(memory_datatype == Datatype{H5T_NATIVE_DOUBLE}) {
        type_id = H5T_IEEE_F64LE;
    }

    assert(type_id >= 0);

    return Datatype(type_id);
}


bool is_native_unsigned_integral(
    Datatype const& datatype)
{
    return std::find(
        native_unsigned_integrals.begin(),
        native_unsigned_integrals.end(),
        datatype) != native_unsigned_integrals.end();
}


bool is_native_signed_integral(
    Datatype const& datatype)
{
    return std::find(
        native_signed_integrals.begin(),
        native_signed_integrals.end(),
        datatype) != native_signed_integrals.end();
}


bool is_native_floating_point(
    Datatype const& datatype)
{
    return std::find(
        native_floating_points.begin(),
        native_floating_points.end(),
        datatype) != native_floating_points.end();
}


bool is_std_unsigned_integral_le(
    Datatype const& datatype)
{
    return std::find(
        std_unsigned_integrals_le.begin(),
        std_unsigned_integrals_le.end(),
        datatype) != std_unsigned_integrals_le.end();
}


bool is_std_signed_integral_le(
    Datatype const& datatype)
{
    return std::find(
        std_signed_integrals_le.begin(),
        std_signed_integrals_le.end(),
        datatype) != std_signed_integrals_le.end();
}


bool is_ieee_floating_point_le(
    Datatype const& datatype)
{
    return std::find(
        ieee_floating_points_le.begin(),
        ieee_floating_points_le.end(),
        datatype) != ieee_floating_points_le.end();
}

} // namespace hdf5
} // namespace lue
