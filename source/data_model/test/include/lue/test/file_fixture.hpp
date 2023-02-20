#pragma once
#include <string>


namespace lue {
    namespace data_model {
        namespace test {

            class FileFixture
            {

                public:

                    explicit FileFixture(
                        std::string const& pathname, bool remove_file_upon_destruction = true);

                    FileFixture(FileFixture const&) = delete;

                    FileFixture(FileFixture&&) = delete;

                    virtual ~FileFixture();

                    FileFixture& operator=(FileFixture const&) = delete;

                    FileFixture& operator=(FileFixture&&) = delete;

                private:

                    void remove_file_if_it_exists();

                    std::string const _pathname;

                    bool const _remove_file_upon_destruction;
            };

        }  // namespace test
    }      // namespace data_model
}  // namespace lue
