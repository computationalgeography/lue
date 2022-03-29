#pragma once
#include "lue/data_model/netcdf4/group.hpp"
#include <netcdf.h>
#include <string>


namespace lue::data_model::netcdf4 {

    // https://docs.unidata.ucar.edu/netcdf-c/current/group__datasets.html
    class File:
        public Group
    {

        public:

            File();

            File(Identifier&& id);

            File(File const&)=delete;

            ~File() override;

            File& operator=(File const&)=delete;

            bool is_open() const;

            void close();

        private:

    };


    File open_file(
        std::string const& pathname,
        int const mode=NC_NOWRITE);

    File create_file(
        std::string const& pathname);

}  // namespace lue::data_model::netcdf4
