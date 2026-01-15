#define BOOST_TEST_MODULE lue hdf5 group
#include "lue/hdf5/file.hpp"
#include "lue/hdf5/group.hpp"
#include "lue/hdf5/test/file_fixture.hpp"
#include <boost/test/included/unit_test.hpp>
#include <utility>


BOOST_AUTO_TEST_CASE(group_names_and_parent_ids)
{
    std::string const filename = "linked_group.h5";
    lue::hdf5::FileFixture fixture{filename};

    auto file = lue::hdf5::create_file(filename);

    // a: /a
    auto group_a = lue::hdf5::create_group(file, "a");
    BOOST_CHECK_EQUAL(group_a.id().name(), "a");
    BOOST_CHECK_EQUAL(group_a.id().pathname(), "/a");
    BOOST_CHECK_EQUAL(group_a.parent().id(), file.id());
    BOOST_CHECK_EQUAL(group_a.parent_pathname(), "/");

    // b: /a/b
    auto group_b = lue::hdf5::create_group(group_a, "b");
    BOOST_CHECK_EQUAL(group_b.id().name(), "b");
    BOOST_CHECK_EQUAL(group_b.id().pathname(), "/a/b");
    BOOST_CHECK_EQUAL(group_b.parent().id(), group_a.id());
    BOOST_CHECK_EQUAL(group_b.parent_pathname(), "/a");

    // c: /a/b/c
    auto group_c = lue::hdf5::create_group(group_b, "c");
    BOOST_CHECK_EQUAL(group_c.id().name(), "c");
    BOOST_CHECK_EQUAL(group_c.id().pathname(), "/a/b/c");
    BOOST_CHECK_EQUAL(group_c.parent().id(), group_b.id());
    BOOST_CHECK_EQUAL(group_c.parent_pathname(), "/a/b");

    // Link d points to group /a/b/c
    // Group d opens the link which points to the group
    // d: /d (→ /a/b/c)
    auto link_d = file.create_soft_link(group_c.id(), "d");
    auto group_d = lue::hdf5::Group{file, "d"};
    BOOST_CHECK_EQUAL(group_d.id().name(), "d");
    BOOST_CHECK_EQUAL(group_d.id().pathname(), "/d");
    BOOST_CHECK_EQUAL(group_d.parent().id(), file.id());
    BOOST_CHECK_EQUAL(group_d.parent_pathname(), "/");

    // Group e opens the group pointed to by the link
    // e: /a/b/c
    auto group_e = lue::hdf5::Group{file, link_d.path()};
    BOOST_CHECK_EQUAL(group_e.id().name(), "c");
    BOOST_CHECK_EQUAL(group_e.id().pathname(), "/a/b/c");
    BOOST_CHECK_EQUAL(group_e.parent().id(), group_b.id());
    BOOST_CHECK_EQUAL(group_e.parent_pathname(), "/a/b");

    // Group f opens a group using an absolute path and an unrelated group
    // f: /a/b/c
    auto group_f = lue::hdf5::Group{group_b, "/a/b/c"};
    BOOST_CHECK_EQUAL(group_f.id().name(), "c");
    BOOST_CHECK_EQUAL(group_f.id().pathname(), "/a/b/c");
    BOOST_CHECK_EQUAL(group_f.parent().id(), group_b.id());
    BOOST_CHECK_EQUAL(group_f.parent_pathname(), "/a/b");

    // Group g opens a group using a relative path and an unrelated group
    // g: /a/b/c
    auto group_g = lue::hdf5::Group{group_b, "./c"};
    BOOST_CHECK_EQUAL(group_g.id().name(), "c");
    BOOST_CHECK_EQUAL(group_g.id().pathname(), "/a/b/c");
    BOOST_CHECK_EQUAL(group_g.parent().id(), group_b.id());
    BOOST_CHECK_EQUAL(group_g.parent_pathname(), "/a/b");
}


BOOST_AUTO_TEST_CASE(object_count)
{
    std::string const filename = "group_object_count.h5";
    lue::hdf5::FileFixture fixture{filename};

    auto file = lue::hdf5::create_file(filename);

    BOOST_CHECK_EQUAL(file.object_count(H5F_OBJ_FILE), 1);
    BOOST_CHECK_EQUAL(file.object_count(H5F_OBJ_DATASET), 0);
    BOOST_CHECK_EQUAL(file.object_count(H5F_OBJ_GROUP), 0);
    BOOST_CHECK_EQUAL(file.object_count(H5F_OBJ_DATATYPE), 0);
    BOOST_CHECK_EQUAL(file.object_count(H5F_OBJ_ATTR), 0);

    {
        auto group_a = lue::hdf5::create_group(file, "a");
        auto group_b = lue::hdf5::create_group(file, "b");
        auto group_c = lue::hdf5::create_group(file, "c");

        BOOST_CHECK_EQUAL(file.object_count(H5F_OBJ_FILE), 1);
        BOOST_CHECK_EQUAL(file.object_count(H5F_OBJ_DATASET), 0);
        BOOST_CHECK_EQUAL(file.object_count(H5F_OBJ_GROUP), 3);
        BOOST_CHECK_EQUAL(file.object_count(H5F_OBJ_DATATYPE), 0);
        BOOST_CHECK_EQUAL(file.object_count(H5F_OBJ_ATTR), 0);
    }

    BOOST_CHECK_EQUAL(file.object_count(H5F_OBJ_FILE), 1);
    BOOST_CHECK_EQUAL(file.object_count(H5F_OBJ_DATASET), 0);
    BOOST_CHECK_EQUAL(file.object_count(H5F_OBJ_GROUP), 0);
    BOOST_CHECK_EQUAL(file.object_count(H5F_OBJ_DATATYPE), 0);
    BOOST_CHECK_EQUAL(file.object_count(H5F_OBJ_ATTR), 0);
}
