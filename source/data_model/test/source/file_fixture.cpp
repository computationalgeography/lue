#include "lue/test/file_fixture.hpp"
#include "lue/hdf5/file.hpp"


namespace lue {
    namespace data_model {
        namespace test {

            FileFixture::FileFixture(std::string const& pathname, bool const remove_file_upon_destruction):

                _pathname{pathname},
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
                if (lue::hdf5::file_exists(_pathname))
                {
                    lue::hdf5::remove_file(_pathname);
                }
            }

        }  // namespace test
    }  // namespace data_model
}  // namespace lue
