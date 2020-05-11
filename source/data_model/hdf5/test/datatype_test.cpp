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
    struct CompareDatatypes
    {
        bool operator()(
            lh5::Datatype const& lhs,
            lh5::Datatype const& rhs) const
        {
            return lhs != rhs && lhs.id() < rhs.id();
        }
    };

    std::map<lh5::Datatype, lh5::Datatype, CompareDatatypes> map
    {
        {lh5::Datatype{H5T_STD_U8LE},   lh5::Datatype{H5T_NATIVE_UINT8}},
        {lh5::Datatype{H5T_STD_U16LE},  lh5::Datatype{H5T_NATIVE_UINT16}},
        {lh5::Datatype{H5T_STD_U32LE},  lh5::Datatype{H5T_NATIVE_UINT32}},
        {lh5::Datatype{H5T_STD_U64LE},  lh5::Datatype{H5T_NATIVE_UINT64}},
        {lh5::Datatype{H5T_STD_I8LE},   lh5::Datatype{H5T_NATIVE_INT8}},
        {lh5::Datatype{H5T_STD_I16LE},  lh5::Datatype{H5T_NATIVE_INT16}},
        {lh5::Datatype{H5T_STD_I32LE},  lh5::Datatype{H5T_NATIVE_INT32}},
        {lh5::Datatype{H5T_STD_I64LE},  lh5::Datatype{H5T_NATIVE_INT64}},
        {lh5::Datatype{H5T_IEEE_F32LE}, lh5::Datatype{H5T_NATIVE_FLOAT}},
        {lh5::Datatype{H5T_IEEE_F64LE}, lh5::Datatype{H5T_NATIVE_DOUBLE}},
        {lh5::create_string_datatype(), lh5::create_string_datatype()}
    };

    BOOST_REQUIRE_EQUAL(map.size(), 11);

    // uint8
    {
        lh5::Datatype const file_datatype{H5T_STD_U8LE};
        lh5::Datatype const memory_datatype{H5T_NATIVE_UINT8};
        auto it = map.find(file_datatype);
        BOOST_REQUIRE(it != map.end());
        BOOST_CHECK(it->second == memory_datatype);
    }

    // int32
    {
        lh5::Datatype const file_datatype{H5T_STD_I32LE};
        lh5::Datatype const memory_datatype{H5T_NATIVE_INT32};
        auto it = map.find(file_datatype);
        BOOST_REQUIRE(it != map.end());
        BOOST_CHECK(it->second == memory_datatype);
    }

    // float32
    {
        lh5::Datatype const file_datatype{H5T_IEEE_F32LE};
        lh5::Datatype const memory_datatype{H5T_NATIVE_FLOAT};
        auto it = map.find(file_datatype);
        BOOST_REQUIRE(it != map.end());
        BOOST_CHECK(it->second == memory_datatype);
    }

    // string
    {
        lh5::Datatype const file_datatype{lh5::create_string_datatype()};
        lh5::Datatype const memory_datatype{lh5::create_string_datatype()};
        auto it = map.find(file_datatype);
        BOOST_REQUIRE(it != map.end());
        BOOST_CHECK(it->second == memory_datatype);
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
