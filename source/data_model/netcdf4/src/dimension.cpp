#include "lue/data_model/netcdf4/dimension.hpp"
#include <netcdf.h>
#include <fmt/format.h>


namespace lue::data_model::netcdf4 {

    namespace {

        std::tuple<std::string, size_t> inquire_dimension(
            Identifier const& group_id,
            Identifier const& dimension_id)
        {
            static char name[NC_MAX_NAME];
            size_t length;
            int const status{::nc_inq_dim(group_id, dimension_id, name, &length)};

            if(status != NC_NOERR)
            {
                throw std::runtime_error("Cannot inquire about dimension");
            }

            return std::make_tuple(std::string{name}, length);
        }

    }  // Anonymous namespace


    Dimension::Dimension(
        Identifier const& group_id,
        Identifier const& dimension_id):

        _id{dimension_id}

    {
        std::tie(_name, _length) = inquire_dimension(group_id, _id);
    }


    Identifier const& Dimension::id() const
    {
        return _id;
    }


    std::string const& Dimension::name() const
    {
        return _name;
    }


    size_t Dimension::length() const
    {
        return _length;
    }


    bool Dimension::operator<(
        Dimension const& other) const
    {
        return _id < other._id;
    }

}  // namespace lue::data_model::netcdf4
