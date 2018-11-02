#define BOOST_TEST_MODULE lue hdf5 datatype
#include <boost/test/unit_test.hpp>
#include "lue/hdf5/datatype.hpp"
#include <map>


BOOST_AUTO_TEST_CASE(native_datatype)
{
    using namespace lue::hdf5;

    Datatype datatype(H5T_NATIVE_HSIZE);

    BOOST_CHECK(datatype.is_native());
}


BOOST_AUTO_TEST_CASE(string_datatype)
{
    using namespace lue::hdf5;

    auto datatype = create_string_datatype();

    BOOST_CHECK(datatype.is_string());
}


BOOST_AUTO_TEST_CASE(equality)
{
    using namespace lue::hdf5;

    // string
    {
        auto datatype1 = create_string_datatype();
        auto datatype2 = create_string_datatype();

        BOOST_CHECK(datatype1 == datatype1);
        BOOST_CHECK(datatype1 == datatype2);
    }

    // native double
    {
        auto datatype1 = native_float64;
        auto datatype2 = native_float64;

        BOOST_CHECK(datatype1 == datatype1);
        BOOST_CHECK(datatype1 == datatype2);
    }

    // std integer
    {
        auto datatype1 = std_int32_le;
        auto datatype2 = std_int32_le;

        BOOST_CHECK(datatype1 == datatype1);
        BOOST_CHECK(datatype1 == datatype2);
    }
}


BOOST_AUTO_TEST_CASE(memory_datatype_by_file_datatype)
{
    using namespace lue::hdf5;

    struct CompareDatatypes
    {
        bool operator()(
            lue::hdf5::Datatype const& lhs,
            lue::hdf5::Datatype const& rhs) const
        {
            return lhs != rhs && lhs.id() < rhs.id();
        }
    };

    std::map<Datatype, Datatype, CompareDatatypes> map
    {
        {Datatype{H5T_STD_U8LE}, Datatype{H5T_NATIVE_UINT8}},
        {Datatype{H5T_STD_U16LE}, Datatype{H5T_NATIVE_UINT16}},
        {Datatype{H5T_STD_U32LE}, Datatype{H5T_NATIVE_UINT32}},
        {Datatype{H5T_STD_U64LE}, Datatype{H5T_NATIVE_UINT64}},
        {Datatype{H5T_STD_I8LE}, Datatype{H5T_NATIVE_INT8}},
        {Datatype{H5T_STD_I16LE}, Datatype{H5T_NATIVE_INT16}},
        {Datatype{H5T_STD_I32LE}, Datatype{H5T_NATIVE_INT32}},
        {Datatype{H5T_STD_I64LE}, Datatype{H5T_NATIVE_INT64}},
        {Datatype{H5T_IEEE_F32LE}, Datatype{H5T_NATIVE_FLOAT}},
        {Datatype{H5T_IEEE_F64LE}, Datatype{H5T_NATIVE_DOUBLE}},
        {create_string_datatype(), create_string_datatype()}
    };

    BOOST_REQUIRE_EQUAL(map.size(), 11);

    // uint8
    {
        Datatype const file_datatype{H5T_STD_U8LE};
        Datatype const memory_datatype{H5T_NATIVE_UINT8};
        auto it = map.find(file_datatype);
        BOOST_REQUIRE(it != map.end());
        BOOST_CHECK(it->second == memory_datatype);
    }

    // int32
    {
        Datatype const file_datatype{H5T_STD_I32LE};
        Datatype const memory_datatype{H5T_NATIVE_INT32};
        auto it = map.find(file_datatype);
        BOOST_REQUIRE(it != map.end());
        BOOST_CHECK(it->second == memory_datatype);
    }

    // float32
    {
        Datatype const file_datatype{H5T_IEEE_F32LE};
        Datatype const memory_datatype{H5T_NATIVE_FLOAT};
        auto it = map.find(file_datatype);
        BOOST_REQUIRE(it != map.end());
        BOOST_CHECK(it->second == memory_datatype);
    }

    // string
    {
        Datatype const file_datatype{create_string_datatype()};
        Datatype const memory_datatype{create_string_datatype()};
        auto it = map.find(file_datatype);
        BOOST_REQUIRE(it != map.end());
        BOOST_CHECK(it->second == memory_datatype);
    }
}


BOOST_AUTO_TEST_CASE(is_valid)
{
    using namespace lue::hdf5;

    // Types created by the application are valid

    // string
    {
        auto datatype = create_string_datatype();
        BOOST_CHECK(datatype.id().is_valid());

        auto datatype_copy = datatype;
        BOOST_CHECK(datatype.id().is_valid());
    }

    // Built-in types are not valid

    // native types
    {
        for(auto const datatype: native_unsigned_integrals) {
            BOOST_CHECK(!datatype.id().is_valid());

            auto datatype_copy = datatype;
            BOOST_CHECK(!datatype.id().is_valid());
        }

        for(auto const datatype: native_signed_integrals) {
            BOOST_CHECK(!datatype.id().is_valid());

            auto datatype_copy = datatype;
            BOOST_CHECK(!datatype.id().is_valid());
        }

        for(auto const datatype: native_floating_points) {
            BOOST_CHECK(!datatype.id().is_valid());

            auto datatype_copy = datatype;
            BOOST_CHECK(!datatype.id().is_valid());
        }
    }

    // std le types
    {
        for(auto const datatype: std_unsigned_integrals_le) {
            BOOST_CHECK(!datatype.id().is_valid());

            auto datatype_copy = datatype;
            BOOST_CHECK(!datatype.id().is_valid());
        }

        for(auto const datatype: std_signed_integrals_le) {
            BOOST_CHECK(!datatype.id().is_valid());

            auto datatype_copy = datatype;
            BOOST_CHECK(!datatype.id().is_valid());
        }

        for(auto const datatype: ieee_floating_points_le) {
            BOOST_CHECK(!datatype.id().is_valid());

            auto datatype_copy = datatype;
            BOOST_CHECK(!datatype.id().is_valid());
        }
    }
}


BOOST_AUTO_TEST_CASE(memory_file_memory_datatype)
{
    using namespace lue::hdf5;

    // uint64
    {
        Datatype memory_datatype1 = native_uint64;
        Datatype memory_datatype2 =
            memory_datatype(file_datatype(memory_datatype1));

        BOOST_CHECK(memory_datatype2 == memory_datatype1);
        BOOST_CHECK(!memory_datatype1.id().is_valid());
        BOOST_CHECK(!memory_datatype2.id().is_valid());
    }

    // float32
    {
        Datatype memory_datatype1 = native_float32;
        Datatype memory_datatype2 =
            memory_datatype(file_datatype(memory_datatype1));

        BOOST_CHECK(memory_datatype2 == memory_datatype1);
        BOOST_CHECK(!memory_datatype1.id().is_valid());
        BOOST_CHECK(!memory_datatype2.id().is_valid());
    }
}
