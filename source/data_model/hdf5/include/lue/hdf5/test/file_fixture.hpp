#pragma once
#include "lue/hdf5/export.hpp"
#include <string>


namespace lue::hdf5 {

    class LUE_HDF5_EXPORT FileFixture
    {

        public:

            FileFixture(std::string pathname, bool remove_file_upon_destruction = true);

            FileFixture(FileFixture const& other) = delete;

            FileFixture(FileFixture&& other) = delete;

            virtual ~FileFixture();

            auto operator=(FileFixture const& other) -> FileFixture& = delete;

            auto operator=(FileFixture&& other) -> FileFixture& = delete;

        private:

            void remove_file_if_it_exists();

            std::string const _pathname;

            bool const _remove_file_upon_destruction;
    };

}  // namespace lue::hdf5
