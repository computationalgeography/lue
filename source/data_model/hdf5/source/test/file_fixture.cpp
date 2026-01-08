#include "lue/hdf5/test/file_fixture.hpp"
#include "lue/hdf5/file.hpp"


namespace lue::hdf5 {

    FileFixture::FileFixture(std::string pathname, bool const remove_file_upon_destruction):

        _pathname{std::move(pathname)},
        _remove_file_upon_destruction{remove_file_upon_destruction}

    {
        // Guarantees:
        // - File does not exist after setup
        remove_file_if_it_exists();
    }


    FileFixture::~FileFixture()
    {
        if (_remove_file_upon_destruction)
        {
            // Guarantees:
            // - File does not exist after teardown
            remove_file_if_it_exists();
        }
    }


    void FileFixture::remove_file_if_it_exists()
    {
        if (file_exists(_pathname))
        {
            remove_file(_pathname);
        }
    }

}  // namespace lue::hdf5
