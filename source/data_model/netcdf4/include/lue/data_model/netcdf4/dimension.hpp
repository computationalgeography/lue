#pragma once
#include "lue/data_model/netcdf4/identifier.hpp"
#include <string>


namespace lue::data_model::netcdf4 {

    // https://docs.unidata.ucar.edu/netcdf-c/current/group__dimensions.html

    class Dimension
    {

        public:

            Dimension(
                Identifier const& group_id,
                Identifier const& dimension_id);

            ~Dimension()=default;

            Identifier const& id() const;

            std::string const& name() const;

            size_t length() const;

            bool operator<(Dimension const& other) const;

        private:

            Identifier _id;

            std::string _name;

            size_t _length;

    };


    using Dimensions = std::vector<Dimension>;

}  // namespace lue::data_model::netcdf4
