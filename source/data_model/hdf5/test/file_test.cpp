#define BOOST_TEST_MODULE lue hdf5 file
#include "lue/hdf5/file.hpp"
#include <boost/test/unit_test.hpp>
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
