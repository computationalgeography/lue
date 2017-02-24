#include "lue/hdf5/datatype.h"
#include "lue/hdf5/datatype_traits.h"
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


template<
    typename Collection>
bool datatype_in(
    hid_t const type_id,
    Collection const& datatypes)
{
    return std::any_of(datatypes.begin(), datatypes.end(),
        [type_id](hid_t const a_type_id) {
            return datatypes_are_equal(type_id, a_type_id);
        }
    );
}

}  // Anonymous namespace


std::string native_datatype_as_string(
    Datatype const& datatype)
{
    assert(datatype.is_native());

    std::string result;

    if(datatypes_are_equal(datatype.id(), H5T_NATIVE_FLOAT)) {
        result = NativeDatatypeTraits<float>::name();
    }
    else if(datatypes_are_equal(datatype.id(), H5T_NATIVE_DOUBLE)) {
        result = NativeDatatypeTraits<double>::name();
    }
    else if(datatypes_are_equal(datatype.id(), H5T_NATIVE_UINT8)) {
        result = NativeDatatypeTraits<uint8_t>::name();
    }
    else if(datatypes_are_equal(datatype.id(), H5T_NATIVE_UINT16)) {
        result = NativeDatatypeTraits<uint16_t>::name();
    }
    else if(datatypes_are_equal(datatype.id(), H5T_NATIVE_UINT32)) {
        result = NativeDatatypeTraits<uint32_t>::name();
    }
    else if(datatypes_are_equal(datatype.id(), H5T_NATIVE_UINT64)) {
        result = NativeDatatypeTraits<uint64_t>::name();
    }
    else if(datatypes_are_equal(datatype.id(), H5T_NATIVE_INT8)) {
        result = NativeDatatypeTraits<int8_t>::name();
    }
    else if(datatypes_are_equal(datatype.id(), H5T_NATIVE_INT16)) {
        result = NativeDatatypeTraits<int16_t>::name();
    }
    else if(datatypes_are_equal(datatype.id(), H5T_NATIVE_INT32)) {
        result = NativeDatatypeTraits<int32_t>::name();
    }
    else if(datatypes_are_equal(datatype.id(), H5T_NATIVE_INT64)) {
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

    if(datatypes_are_equal(datatype.id(), H5T_STD_I8LE)) {
        result = StandardDatatypeTraits<int8_t>::name();
    }
    else if(datatypes_are_equal(datatype.id(), H5T_STD_I8BE)) {
        result = "H5T_STD_I8BE";
    }
    else if(datatypes_are_equal(datatype.id(), H5T_STD_I16LE)) {
        result = StandardDatatypeTraits<int16_t>::name();
    }
    else if(datatypes_are_equal(datatype.id(), H5T_STD_I16BE)) {
        result = "H5T_STD_I16BE";
    }
    else if(datatypes_are_equal(datatype.id(), H5T_STD_I32LE)) {
        result = StandardDatatypeTraits<int32_t>::name();
    }
    else if(datatypes_are_equal(datatype.id(), H5T_STD_I32BE)) {
        result = "H5T_STD_I32BE";
    }
    else if(datatypes_are_equal(datatype.id(), H5T_STD_I64LE)) {
        result = StandardDatatypeTraits<int64_t>::name();
    }
    else if(datatypes_are_equal(datatype.id(), H5T_STD_I64BE)) {
        result = "H5T_STD_I64BE";
    }
    else if(datatypes_are_equal(datatype.id(), H5T_STD_U8LE)) {
        result = StandardDatatypeTraits<uint8_t>::name();
    }
    else if(datatypes_are_equal(datatype.id(), H5T_STD_U8BE)) {
        result = "H5T_STD_U8BE";
    }
    else if(datatypes_are_equal(datatype.id(), H5T_STD_U16LE)) {
        result = StandardDatatypeTraits<uint16_t>::name();
    }
    else if(datatypes_are_equal(datatype.id(), H5T_STD_U16BE)) {
        result = "H5T_STD_U16BE";
    }
    else if(datatypes_are_equal(datatype.id(), H5T_STD_U32LE)) {
        result = StandardDatatypeTraits<uint32_t>::name();
    }
    else if(datatypes_are_equal(datatype.id(), H5T_STD_U32BE)) {
        result = "H5T_STD_U32BE";
    }
    else if(datatypes_are_equal(datatype.id(), H5T_STD_U64LE)) {
        result = StandardDatatypeTraits<uint64_t>::name();
    }
    else if(datatypes_are_equal(datatype.id(), H5T_STD_U64BE)) {
        result = "H5T_STD_U64BE";
    }
    else if(datatypes_are_equal(datatype.id(), H5T_IEEE_F32LE)) {
        result = StandardDatatypeTraits<float>::name();
    }
    else if(datatypes_are_equal(datatype.id(), H5T_IEEE_F32BE)) {
        result = "H5T_STD_F32BE";
    }
    else if(datatypes_are_equal(datatype.id(), H5T_IEEE_F64LE)) {
        result = StandardDatatypeTraits<double>::name();
    }
    else if(datatypes_are_equal(datatype.id(), H5T_IEEE_F64BE)) {
        result = "H5T_STD_F64BE";
    }

    assert(!result.empty());

    return result;
}


// TODO Turn into create_datatype()
Datatype::Datatype(
    size_t const nr_bytes)

    : _id(::H5Tcopy(H5T_C_S1), ::H5Tclose)

{
    if(!_id.is_valid()) {
        throw std::runtime_error("Cannot create datatype");
    }


    auto status = ::H5Tset_cset(_id, H5T_CSET_UTF8);

    if(status < 0) {
        throw std::runtime_error("Cannot set character set");
    }

    set_size(nr_bytes);
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

    : _id(id, [](hid_t){})

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


void Datatype::set_size(size_t const nr_bytes)
{
    auto status = ::H5Tset_size(_id, nr_bytes);

    if(status < 0) {
        throw std::runtime_error("Cannot set size");
    }
}


size_t Datatype::size() const
{
    return ::H5Tget_size(_id);
}


/*!
    @brief      Retrieve the encoding of a string datatype
    @param      .
    @return     .
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
    static std::set<hid_t> const standard_datatypes = {
        H5T_STD_I8LE,
        H5T_STD_I16LE,
        H5T_STD_I32LE,
        H5T_STD_I64LE,
        H5T_STD_U8LE,
        H5T_STD_U16LE,
        H5T_STD_U32LE,
        H5T_STD_U64LE,
        H5T_IEEE_F32LE,
        H5T_IEEE_F64LE
    };

    return datatype_in(_id, standard_datatypes);
}




bool Datatype::is_native() const
{
    static std::set<hid_t> const native_datatypes = {
        H5T_NATIVE_FLOAT,
        H5T_NATIVE_DOUBLE,
        H5T_NATIVE_UINT8,
        H5T_NATIVE_UINT16,
        H5T_NATIVE_UINT32,
        H5T_NATIVE_UINT64,
        H5T_NATIVE_INT8,
        H5T_NATIVE_INT16,
        H5T_NATIVE_INT32,
        H5T_NATIVE_INT64
    };

    return datatype_in(_id, native_datatypes);
}


bool operator==(
    Datatype const& lhs,
    Datatype const& rhs)
{
    return datatypes_are_equal(lhs.id(), rhs.id());
}


bool operator!=(
    Datatype const& lhs,
    Datatype const& rhs)
{
    return !datatypes_are_equal(lhs.id(), rhs.id());
}

} // namespace hdf5
} // namespace lue
