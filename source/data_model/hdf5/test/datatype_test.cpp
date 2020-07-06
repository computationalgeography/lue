#define BOOST_TEST_MODULE lue hdf5 datatype
#include <boost/test/unit_test.hpp>
#include "lue/hdf5/datatype.hpp"
#include <map>


namespace lh5 = lue::hdf5;


BOOST_AUTO_TEST_CASE(native_datatype)
{
    lh5::Datatype datatype(H5T_NATIVE_HSIZE);

    BOOST_CHECK(datatype.is_native());
}


BOOST_AUTO_TEST_CASE(string_datatype)
{
    auto datatype = lh5::create_string_datatype();

    BOOST_CHECK(datatype.is_string());
}


BOOST_AUTO_TEST_CASE(equality)
{
    // string
    {
        auto datatype1 = lh5::create_string_datatype();
        auto datatype2 = lh5::create_string_datatype();

        BOOST_CHECK(datatype1 == datatype1);
        BOOST_CHECK(datatype1 == datatype2);
    }

    // native double
    {
        auto datatype1 = lh5::native_float64;
        auto datatype2 = lh5::native_float64;

        BOOST_CHECK(datatype1 == datatype1);
        BOOST_CHECK(datatype1 == datatype2);
    }

    // std integer
    {
        auto datatype1 = lh5::std_int32_le;
        auto datatype2 = lh5::std_int32_le;

        BOOST_CHECK(datatype1 == datatype1);
        BOOST_CHECK(datatype1 == datatype2);
    }
}


BOOST_AUTO_TEST_CASE(memory_datatype_by_file_datatype)
{
    BOOST_CHECK(lh5::memory_datatype(lh5::std_uint8_le) == lh5::native_uint8);
    BOOST_CHECK(lh5::memory_datatype(lh5::std_uint16_le) == lh5::native_uint16);
    BOOST_CHECK(lh5::memory_datatype(lh5::std_uint32_le) == lh5::native_uint32);
    BOOST_CHECK(lh5::memory_datatype(lh5::std_uint64_le) == lh5::native_uint64);
    BOOST_CHECK(lh5::memory_datatype(lh5::std_int8_le) == lh5::native_int8);
    BOOST_CHECK(lh5::memory_datatype(lh5::std_int16_le) == lh5::native_int16);
    BOOST_CHECK(lh5::memory_datatype(lh5::std_int32_le) == lh5::native_int32);
    BOOST_CHECK(lh5::memory_datatype(lh5::std_int64_le) == lh5::native_int64);
    BOOST_CHECK(lh5::memory_datatype(lh5::ieee_float32_le) == lh5::native_float32);
    BOOST_CHECK(lh5::memory_datatype(lh5::ieee_float64_le) == lh5::native_float64);

    lh5::Datatype const string_datatype{lh5::create_string_datatype()};
    BOOST_CHECK(lh5::memory_datatype(string_datatype) == string_datatype);

    // Use newly created types should also work
    // uint8
    {
        lh5::Datatype const file_datatype{H5T_STD_U8LE};
        lh5::Datatype const memory_datatype{H5T_NATIVE_UINT8};
        BOOST_CHECK(lh5::file_datatype(memory_datatype) == file_datatype);
    }
}


BOOST_AUTO_TEST_CASE(file_datatype_by_memory_datatype)
{
    // Use predefined types should work
    BOOST_CHECK(lh5::file_datatype(lh5::native_uint8) == lh5::std_uint8_le);
    BOOST_CHECK(lh5::file_datatype(lh5::native_uint16) == lh5::std_uint16_le);
    BOOST_CHECK(lh5::file_datatype(lh5::native_uint32) == lh5::std_uint32_le);
    BOOST_CHECK(lh5::file_datatype(lh5::native_uint64) == lh5::std_uint64_le);
    BOOST_CHECK(lh5::file_datatype(lh5::native_int8) == lh5::std_int8_le);
    BOOST_CHECK(lh5::file_datatype(lh5::native_int16) == lh5::std_int16_le);
    BOOST_CHECK(lh5::file_datatype(lh5::native_int32) == lh5::std_int32_le);
    BOOST_CHECK(lh5::file_datatype(lh5::native_int64) == lh5::std_int64_le);
    BOOST_CHECK(lh5::file_datatype(lh5::native_float32) == lh5::ieee_float32_le);
    BOOST_CHECK(lh5::file_datatype(lh5::native_float64) == lh5::ieee_float64_le);

    lh5::Datatype const string_datatype{lh5::create_string_datatype()};
    BOOST_CHECK(lh5::file_datatype(string_datatype) == string_datatype);

    // Use newly created types should also work
    // uint8
    {
        lh5::Datatype const file_datatype{H5T_STD_U8LE};
        lh5::Datatype const memory_datatype{H5T_NATIVE_UINT8};
        BOOST_CHECK(lh5::file_datatype(memory_datatype) == file_datatype);
    }
}


BOOST_AUTO_TEST_CASE(is_valid)
{
    // Types created by the application are valid

    // string
    {
        auto datatype = lh5::create_string_datatype();
        BOOST_CHECK(datatype.id().is_valid());

        // NOLINTNEXTLINE(performance-unnecessary-copy-initialization)
        auto datatype_copy = datatype;
        BOOST_CHECK(datatype_copy.id().is_valid());
    }

    // Built-in types are not valid

    // native types
    {
        for(auto const& datatype: lh5::native_unsigned_integrals) {
            BOOST_CHECK(!datatype.id().is_valid());

            // NOLINTNEXTLINE(performance-unnecessary-copy-initialization)
            auto datatype_copy = datatype;
            BOOST_CHECK(!datatype_copy.id().is_valid());
        }

        for(auto const& datatype: lh5::native_signed_integrals) {
            BOOST_CHECK(!datatype.id().is_valid());

            // NOLINTNEXTLINE(performance-unnecessary-copy-initialization)
            auto datatype_copy = datatype;
            BOOST_CHECK(!datatype_copy.id().is_valid());
        }

        for(auto const& datatype: lh5::native_floating_points) {
            BOOST_CHECK(!datatype.id().is_valid());

            // NOLINTNEXTLINE(performance-unnecessary-copy-initialization)
            auto datatype_copy = datatype;
            BOOST_CHECK(!datatype_copy.id().is_valid());
        }
    }

    // std le types
    {
        for(auto const& datatype: lh5::std_unsigned_integrals_le) {
            BOOST_CHECK(!datatype.id().is_valid());

            // NOLINTNEXTLINE(performance-unnecessary-copy-initialization)
            auto datatype_copy = datatype;
            BOOST_CHECK(!datatype_copy.id().is_valid());
        }

        for(auto const& datatype: lh5::std_signed_integrals_le) {
            BOOST_CHECK(!datatype.id().is_valid());

            // NOLINTNEXTLINE(performance-unnecessary-copy-initialization)
            auto datatype_copy = datatype;
            BOOST_CHECK(!datatype_copy.id().is_valid());
        }

        for(auto const& datatype: lh5::ieee_floating_points_le) {
            BOOST_CHECK(!datatype.id().is_valid());

            // NOLINTNEXTLINE(performance-unnecessary-copy-initialization)
            auto datatype_copy = datatype;
            BOOST_CHECK(!datatype_copy.id().is_valid());
        }
    }
}


BOOST_AUTO_TEST_CASE(memory_file_memory_datatype)
{
    // uint64
    {
        lh5::Datatype memory_datatype1 = lh5::native_uint64;
        lh5::Datatype memory_datatype2 =
            memory_datatype(file_datatype(memory_datatype1));

        BOOST_CHECK(memory_datatype2 == memory_datatype1);
        BOOST_CHECK(!memory_datatype1.id().is_valid());
        BOOST_CHECK(!memory_datatype2.id().is_valid());
    }

    // float32
    {
        lh5::Datatype memory_datatype1 = lh5::native_float32;
        lh5::Datatype memory_datatype2 =
            memory_datatype(file_datatype(memory_datatype1));

        BOOST_CHECK(memory_datatype2 == memory_datatype1);
        BOOST_CHECK(!memory_datatype1.id().is_valid());
        BOOST_CHECK(!memory_datatype2.id().is_valid());
    }
}
