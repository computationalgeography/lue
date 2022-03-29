#include "lue/data_model/netcdf4/file.hpp"
#include <fmt/format.h>
#include <cassert>
#include <stdexcept>


namespace lue::data_model::netcdf4 {

    File::File():

        Group{}

    {
    }


    File::File(
        Identifier&& id):

        Group{std::move(id)}

    {
    }


    File::~File()
    {
        if(is_open())
        {
            close();
        }
    }


    bool File::is_open() const
    {
        return id().is_valid();
    }


    void File::close()
    {
        assert(is_open());

        int status{::nc_close(id())};

        if(status != NC_NOERR)
        {
            throw std::runtime_error("Cannot close NetCDF4 file");
        }

        id().invalidate();

        assert(!is_open());
    }


    File open_file(
        std::string const& pathname,
        int const mode)
    {
        int id;
        int status = ::nc_open(pathname.c_str(), mode, &id);

        if(status != NC_NOERR)
        {
            throw std::runtime_error(fmt::format("Cannot open NetCDF4 dataset {}", pathname));
        }

        return File{Identifier{id}};
    }


    File create_file(
        std::string const& pathname)
    {
        int id;
        int status = ::nc_create(pathname.c_str(), NC_CLOBBER, &id);

        if(status != NC_NOERR)
        {
            throw std::runtime_error(fmt::format("Cannot create NetCDF4 dataset {}", pathname));
        }

        return File{Identifier{id}};
    }

}  // namespace lue::data_model::netcdf4
