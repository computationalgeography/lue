#pragma once
#include "lue/data_model/netcdf4/identifier.hpp"
#include <netcdf.h>
#include <string>


namespace lue::data_model::netcdf4 {

    // https://docs.unidata.ucar.edu/netcdf-c/current/group__attributes.html


    class Attribute
    {

        public:

            Attribute(
                Identifier const& group_id,
                Identifier const& attribute_id);

            Attribute(
                Identifier const& group_id,
                Identifier const& variable_id,
                Identifier const& attribute_id);

            ~Attribute()=default;

            std::string const& name() const;

            nc_type data_type() const;

        private:

            Identifier _id;

            std::string _name;

            size_t _length;

            nc_type _data_type;

    };


    using Attributes = std::vector<Attribute>;


    // std::string attribute(
    //     Identifier const& group,
    //     std::string const& name);

    // template<
    //     typename T>
    // std::vector<T> attribute(
    //     Identifier const& group,
    //     std::string const& name);

    // template<
    //     typename T>
    // std::vector<T> attribute(
    //     Identifier const& group,
    //     Identifier const& variable,
    //     std::string const& name);

}  // namespace lue::data_model::netcdf4
