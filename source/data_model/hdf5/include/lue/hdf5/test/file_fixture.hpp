#pragma once
#include "lue/hdf5/export.hpp"
#include <string>


namespace lue::hdf5 {

    class LUE_HDF5_EXPORT FileFixture
    {

        public:

            FileFixture(std::string const& pathname, bool remove_file_upon_destruction = true);

            FileFixture(FileFixture const&) = delete;

            FileFixture(FileFixture&&) = delete;

            virtual ~FileFixture();

            auto operator=(FileFixture const&) -> FileFixture& = delete;

            auto operator=(FileFixture&&) -> FileFixture& = delete;

        private:

            void remove_file_if_it_exists();

            std::string const _pathname;

            bool const _remove_file_upon_destruction;
    };

}  // namespace lue::hdf5
