#define BOOST_TEST_MODULE lue hdf5 identifier
#include "lue/hdf5/file.hpp"
#include "lue/hdf5/identifier.hpp"
#include <boost/test/unit_test.hpp>


namespace lh5 = lue::hdf5;


BOOST_AUTO_TEST_CASE(default_construct)
{
    lh5::Identifier identifier{};

    BOOST_CHECK(!identifier.is_valid());
}


BOOST_AUTO_TEST_CASE(reference_count)
{
    lh5::File::AccessPropertyList access_property_list{};
    access_property_list.use_core_driver();

    // id -> id
    {
        lh5::Identifier id1{
            ::H5Fcreate("my_file", H5F_ACC_TRUNC, H5P_DEFAULT, access_property_list.id()), ::H5Fclose};

        BOOST_CHECK(id1.is_valid());
        BOOST_CHECK_EQUAL(id1.reference_count(), 1);

        // Move id1 to id2
        lh5::Identifier id2{std::move(id1)};
        BOOST_CHECK(!id1.is_valid());
        BOOST_CHECK(id2.is_valid());
        BOOST_CHECK_EQUAL(id2.reference_count(), 1);

        {
            // Copy id2 to id3
            lh5::Identifier id3{id2};
            BOOST_CHECK(id2.is_valid());
            BOOST_CHECK(id3.is_valid());
            BOOST_CHECK_EQUAL(id2.reference_count(), 2);
            BOOST_CHECK_EQUAL(id3.reference_count(), 2);
        }

        BOOST_CHECK_EQUAL(id2.reference_count(), 1);
    }

    // id -> group
    {
        lh5::Group group{lh5::Identifier{
            ::H5Fcreate("my_file", H5F_ACC_TRUNC, H5P_DEFAULT, access_property_list.id()), ::H5Fclose}};

        // Each group has attributes, which stores a copy of the identifier
        BOOST_CHECK_EQUAL(group.id().reference_count(), 2);
    }

    // id -> file
    {
        lh5::File file{lh5::Identifier{
            ::H5Fcreate("my_file", H5F_ACC_TRUNC, H5P_DEFAULT, access_property_list.id()), ::H5Fclose}};

        // Each file is a group
        BOOST_CHECK_EQUAL(file.id().reference_count(), 2);
    }
}
