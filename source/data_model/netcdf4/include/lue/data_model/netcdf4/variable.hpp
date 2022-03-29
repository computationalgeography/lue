#pragma once
#include "lue/data_model/netcdf4/attribute.hpp"
#include "lue/data_model/netcdf4/dimension.hpp"
#include <netcdf.h>
#include <string>


namespace lue::data_model::netcdf4 {

    // https://docs.unidata.ucar.edu/netcdf-c/current/group__dimensions.html

    class Variable
    {

        public:

            Variable(
                Identifier const& group_id,
                Identifier const& variable_id);

            ~Variable()=default;

            std::string const& name() const;

            Attributes const& attributes() const;

            Dimensions const& dimensions() const;

            nc_type data_type() const;

            bool is_coordinate_variable() const;

        private:

            Identifier _id;

            std::string _name;

            nc_type _data_type;

            Attributes _attributes;

            Dimensions _dimensions;

    };


    using Variables = std::vector<Variable>;

}  // namespace lue::data_model::netcdf4
