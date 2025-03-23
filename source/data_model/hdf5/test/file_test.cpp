#define BOOST_TEST_MODULE lue hdf5 file
#include "lue/hdf5/file.hpp"
#include <boost/test/included/unit_test.hpp>
#include <filesystem>


namespace lh5 = lue::hdf5;


BOOST_AUTO_TEST_CASE(try_write_to_read_online_file)
{
    // Turn off HDF5 stack trace printing
    H5Eset_auto(H5E_DEFAULT, nullptr, nullptr);

    std::string name{"try_write_to_read_online_file.h5"};

    // Remove any lingering file
    if (lh5::file_exists(name))
    {
        std::filesystem::permissions(name, std::filesystem::perms::owner_write);
        lh5::remove_file(name);
    }

    // Create file and make it read-only
    lh5::create_file(name);
    std::filesystem::permissions(name, std::filesystem::perms::owner_read);

    // Verify our exception gets thrown
    BOOST_CHECK_THROW((lh5::File{name, H5F_ACC_RDWR}), std::runtime_error);

    std::filesystem::permissions(name, std::filesystem::perms::owner_write);
}


BOOST_AUTO_TEST_CASE(use_moved_file)
{
    std::string name{"use_moved_file.h5"};

    lh5::File::AccessPropertyList access_property_list = lh5::File::AccessPropertyList{};
    access_property_list.set_library_version_bounds(H5F_LIBVER_LATEST, H5F_LIBVER_LATEST);

    auto file1 = lh5::create_file(name, std::move(access_property_list));

    file1.attributes().write<std::string>("attr1", "attr1");
    file1.attributes().write<std::string>("attr2", "attr2");

    auto file2 = std::move(file1);

    file2.attributes().write<std::string>("attr3", "attr3");
    file2.attributes().write<std::string>("attr4", "attr4");
}
