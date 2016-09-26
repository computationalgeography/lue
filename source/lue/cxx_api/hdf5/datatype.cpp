#include "lue/cxx_api/hdf5/datatype.h"
#include "lue/cxx_api/hdf5/datatype_traits.h"
#include <algorithm>
#include <cassert>
#include <stdexcept>
#include <iostream>
#include <set>


namespace lue {

bool datatypes_are_equal(
    hid_t const type_id1,
    hid_t const type_id2)
{
    auto const result = H5Tequal(type_id1, type_id2);
    assert(result >= 0);

    return result > 0;
}


std::string native_datatype_as_string(
    hid_t const type_id)
{
    std::string result;

    if(datatypes_are_equal(type_id, H5T_NATIVE_FLOAT)) {
        result = NativeDatatypeTraits<float>::name();
    }
    else if(datatypes_are_equal(type_id, H5T_NATIVE_DOUBLE)) {
        result = NativeDatatypeTraits<double>::name();
    }
    else if(datatypes_are_equal(type_id, H5T_NATIVE_UINT32)) {
        result = NativeDatatypeTraits<uint32_t>::name();
    }
    else if(datatypes_are_equal(type_id, H5T_NATIVE_UINT64)) {
        result = NativeDatatypeTraits<uint64_t>::name();
    }
    else if(datatypes_are_equal(type_id, H5T_NATIVE_INT32)) {
        result = NativeDatatypeTraits<int32_t>::name();
    }
    else if(datatypes_are_equal(type_id, H5T_NATIVE_INT64)) {
        result = NativeDatatypeTraits<int64_t>::name();
    }

    assert(!result.empty());

    return result;
}


std::string standard_datatype_as_string(
    hid_t const type_id)
{
    std::string result;

    if(datatypes_are_equal(type_id, H5T_STD_I32LE)) {
        result = StandardDatatypeTraits<int32_t>::name();
    }
    else if(datatypes_are_equal(type_id, H5T_STD_I64LE)) {
        result = StandardDatatypeTraits<int64_t>::name();
    }
    else if(datatypes_are_equal(type_id, H5T_STD_U32LE)) {
        result = StandardDatatypeTraits<uint32_t>::name();
    }
    else if(datatypes_are_equal(type_id, H5T_STD_U64LE)) {
        result = StandardDatatypeTraits<uint64_t>::name();
    }
    else if(datatypes_are_equal(type_id, H5T_IEEE_F32LE)) {
        result = StandardDatatypeTraits<float>::name();
    }
    else if(datatypes_are_equal(type_id, H5T_IEEE_F64LE)) {
        result = StandardDatatypeTraits<double>::name();
    }

    assert(!result.empty());

    return result;
}


hid_t parse_datatype(
    std::string const& string)
{
    hid_t result = -1;

    if(string == StandardDatatypeTraits<int32_t>::name()) {
        result = StandardDatatypeTraits<int32_t>::type_id();
    }
    else if(string == StandardDatatypeTraits<float>::name()) {
        result = StandardDatatypeTraits<float>::type_id();
    }
    else {
        throw std::runtime_error("Datatype " + string + " cannot be parsed");
    }

    assert(result >= 0);

    return result;
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


bool is_standard_datatype(
    hid_t const type_id)
{
    static std::set<hid_t> const standard_datatypes = {
        H5T_STD_I32LE,
        H5T_STD_I64LE,
        H5T_STD_U32LE,
        H5T_STD_U64LE,
        H5T_IEEE_F32LE,
        H5T_IEEE_F64LE
    };

    return datatype_in(type_id, standard_datatypes);
}


bool is_native_datatype(
    hid_t const type_id)
{
    static std::set<hid_t> const native_datatypes = {
        H5T_NATIVE_FLOAT,
        H5T_NATIVE_DOUBLE,
        H5T_NATIVE_UINT32,
        H5T_NATIVE_UINT64,
        H5T_NATIVE_INT32,
        H5T_NATIVE_INT64
    };

    return datatype_in(type_id, native_datatypes);
}


hid_t native_type_id(
    hid_t const file_type_id)
{
    assert(is_standard_datatype(file_type_id));

    hid_t result = -1;

    if(datatypes_are_equal(file_type_id, H5T_STD_I32LE)) {
        result = H5T_NATIVE_INT32;
    }
    else if(datatypes_are_equal(file_type_id, H5T_STD_I64LE)) {
        result = H5T_NATIVE_INT64;
    }
    else if(datatypes_are_equal(file_type_id, H5T_STD_U32LE)) {
        result = H5T_NATIVE_UINT32;
    }
    else if(datatypes_are_equal(file_type_id, H5T_STD_U64LE)) {
        result = H5T_NATIVE_UINT64;
    }
    else if(datatypes_are_equal(file_type_id, H5T_IEEE_F32LE)) {
        result = H5T_NATIVE_FLOAT;
    }
    else if(datatypes_are_equal(file_type_id, H5T_IEEE_F64LE)) {
        result = H5T_NATIVE_DOUBLE;
    }

    assert(result >= 0);
    assert(is_native_datatype(result));

    return result;
}

}  // namespace lue
