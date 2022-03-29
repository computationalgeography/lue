#pragma once
#include "lue/data_model/netcdf4/attribute.hpp"
#include "lue/data_model/netcdf4/dimension.hpp"
#include "lue/data_model/netcdf4/variable.hpp"
#include <netcdf.h>
#include <tuple>


namespace lue::data_model::netcdf4 {

    class Group;

    using Groups = std::vector<Group>;


    // https://docs.unidata.ucar.edu/netcdf-c/current/group__groups.html
    class Group
    {

        public:

            Group();

            Group(Identifier&& id);

            Group(Group const&)=delete;

            Group(Group&&)=default;

            virtual ~Group()=default;

            Group& operator=(Group const&)=delete;

            Group& operator=(Group&&)=default;

            Attributes const& attributes() const;

            Dimensions const& dimensions() const;

            // Dimension const& dimension(Identifier const& id) const;

            Variables const& variables() const;

            bool contains_variable(std::string const& name) const;

            Variable const& variable(std::string const& name) const;

            Groups const& groups() const;

            // std::string attribute(std::string const& name) const;

            // template<
            //     typename T>
            // std::vector<T> attribute(
            //     std::string const& name) const
            // {
            //     return lue::data_model::netcdf4::attribute<T>(_id, name);
            // }

        protected:

            Identifier const& id() const;

            Identifier& id();

        private:

            Variables::const_iterator find_variable(std::string const& name) const;

            Identifier _id;

            Attributes _attributes;

            Dimensions _dimensions;

            Variables _variables;

            Groups _groups;

    };


}  // namespace lue::data_model::netcdf4
