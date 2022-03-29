#include "lue/data_model/netcdf4/group.hpp"
#include <fmt/format.h>
#include <cassert>
#include <utility>


namespace lue::data_model::netcdf4 {

    namespace {

        std::tuple<int, int, int> inquire(
            Identifier const& group_id)
        {
            int nr_dimensions;
            int nr_variables;
            int nr_attributes;
            int const status{::nc_inq(group_id, &nr_dimensions, &nr_variables, &nr_attributes, nullptr)};

            if(status != NC_NOERR)
            {
                throw std::runtime_error("Cannot inquire about group");
            }

            return std::make_tuple(nr_dimensions, nr_variables, nr_attributes);
        }


        // std::string Group::attribute_name(
        //     int const id) const
        // {
        //     static char name[NC_MAX_NAME];
        //     int const status{::nc_inq_attname(_id, NC_GLOBAL, id, name)};

        //     if(status != NC_NOERR)
        //     {
        //         throw std::runtime_error(fmt::format("Cannot obtain name of attribute {}", id));
        //     }

        //     // This assumes the name is null-terminated
        //     return std::string{name};
        // }


        // nc_type Group::attribute_type(
        //     std::string const& name) const
        // {
        //     nc_type data_type;
        //     int const status{::nc_inq_atttype(_id, NC_GLOBAL, name.c_str(), &data_type)};

        //     if(status != NC_NOERR)
        //     {
        //         throw std::runtime_error(fmt::format("Cannot obtain type of attribute {}", name));
        //     }

        //     return data_type;
        // }


        // std::string Group::attribute(
        //     std::string const& name) const
        // {
        //     return lue::data_model::netcdf4::attribute(_id, name);
        // }


        // std::tuple<std::string, size_t> Group::inquire_dimension(
        //     int const id) const
        // {
        //     static char name[NC_MAX_NAME];
        //     size_t length;
        //     int const status{::nc_inq_dim(_id, id, name, &length)};

        //     if(status != NC_NOERR)
        //     {
        //         throw std::runtime_error(fmt::format("Cannot inquire about dimension {}", id));
        //     }

        //     return std::make_tuple(std::string{name}, length);
        // }


        // std::tuple<std::string, nc_type, std::vector<int>, int> Group::inquire_variable(
        //     int const id) const
        // {
        //     static char name[NC_MAX_NAME];
        //     nc_type data_type;
        //     int nr_dimensions;
        //     static int dimension_ids[NC_MAX_VAR_DIMS];
        //     int nr_attributes;

        //     int const status{
        //         ::nc_inq_var(_id, id, name, &data_type, &nr_dimensions, dimension_ids, &nr_attributes)};

        //     if(status != NC_NOERR)
        //     {
        //         throw std::runtime_error(fmt::format("Cannot inquire about dimension {}", id));
        //     }

        //     return std::make_tuple(
        //             std::string{name}, data_type, std::vector<int>(dimension_ids, dimension_ids + nr_dimensions),
        //             nr_attributes
        //         );
        // }


        Identifiers group_ids(
            Identifier const& group_id)
        {
            int nr_groups;
            static int group_ids[32767];  // TODO According to docs. Which constant to use?

            int const status{::nc_inq_grps(group_id, &nr_groups, group_ids)};

            if(status != NC_NOERR)
            {
                throw std::runtime_error("Cannot obtain group IDs");
            }

            return Identifiers(group_ids, group_ids + nr_groups);
        }

    }  // Anonymous namespace


    Group::Group():

        _id{},
        _attributes{},
        _dimensions{},
        _variables{}

    {
    }


    Group::Group(
        Identifier&& id):

        _id{std::move(id)},
        _attributes{},
        _dimensions{},
        _variables{}

    {
        if(_id.is_valid())
        {
            auto const [nr_dimensions, nr_variables, nr_attributes] = inquire(_id);

            _attributes.reserve(nr_attributes);
            for(int i = 0; i < nr_attributes; ++i)
            {
                _attributes.push_back(Attribute{_id, i});
            }

            _dimensions.reserve(nr_dimensions);
            for(int i = 0; i < nr_dimensions; ++i)
            {
                _dimensions.push_back(Dimension{_id, i});
            }

            _variables.reserve(nr_variables);
            for(int i = 0; i < nr_variables; ++i)
            {
                _variables.push_back(Variable{_id, i});
            }

            Identifiers group_ids_{group_ids(_id)};
            _groups.reserve(group_ids_.size());
            for(std::size_t i = 0; i < group_ids_.size(); ++i)
            {
                _groups.push_back(Group{std::move(group_ids_[i])});
            }
        }
    }


    Identifier const& Group::id() const
    {
        return _id;
    }


    Identifier& Group::id()
    {
        return _id;
    }


    Attributes const& Group::attributes() const
    {
        return _attributes;
    }


    Dimensions const& Group::dimensions() const
    {
        return _dimensions;
    }


    // Dimension const& Group::dimension(
    //     Identifier const& id) const
    // {
    //     auto it = std::find_if(_dimensions.begin(), _dimensions.end(),
    //             [id](Dimension const& dimension)
    //             {
    //                 return dimension.id() == id;
    //             }
    //         );

    //     assert(it != _dimensions.end());

    //     if(it == _dimensions.end())
    //     {
    //         throw std::runtime_error("Cannot find requested dimension");
    //     }

    //     return *it;
    // }


    Variables const& Group::variables() const
    {
        return _variables;
    }


    Variables::const_iterator Group::find_variable(
        std::string const& name) const
    {
        return std::find_if(_variables.begin(), _variables.end(),
                [name](Variable const& variable)
                {
                    return variable.name() == name;
                }
            );
    }


    bool Group::contains_variable(
        std::string const& name) const
    {
        return find_variable(name) != _variables.end();
    }


    Variable const& Group::variable(
        std::string const& name) const
    {
        return *find_variable(name);
    }


    Groups const& Group::groups() const
    {
        return _groups;
    }




    // std::tuple<int, int, int> Group::inquire() const
    // {
    //     int nr_dimensions;
    //     int nr_variables;
    //     int nr_attributes;
    //     int const status{::nc_inq(_id, &nr_dimensions, &nr_variables, &nr_attributes, nullptr)};

    //     if(status != NC_NOERR)
    //     {
    //         throw std::runtime_error("Cannot inquire about group");
    //     }

    //     return std::make_tuple(nr_dimensions, nr_variables, nr_attributes);
    // }


    // std::string Group::attribute_name(
    //     int const id) const
    // {
    //     static char name[NC_MAX_NAME];
    //     int const status{::nc_inq_attname(_id, NC_GLOBAL, id, name)};

    //     if(status != NC_NOERR)
    //     {
    //         throw std::runtime_error(fmt::format("Cannot obtain name of attribute {}", id));
    //     }

    //     // This assumes the name is null-terminated
    //     return std::string{name};
    // }


    // nc_type Group::attribute_type(
    //     std::string const& name) const
    // {
    //     nc_type data_type;
    //     int const status{::nc_inq_atttype(_id, NC_GLOBAL, name.c_str(), &data_type)};

    //     if(status != NC_NOERR)
    //     {
    //         throw std::runtime_error(fmt::format("Cannot obtain type of attribute {}", name));
    //     }

    //     return data_type;
    // }


    // std::string Group::attribute(
    //     std::string const& name) const
    // {
    //     return lue::data_model::netcdf4::attribute(_id, name);
    // }


    // std::tuple<std::string, size_t> Group::inquire_dimension(
    //     int const id) const
    // {
    //     static char name[NC_MAX_NAME];
    //     size_t length;
    //     int const status{::nc_inq_dim(_id, id, name, &length)};

    //     if(status != NC_NOERR)
    //     {
    //         throw std::runtime_error(fmt::format("Cannot inquire about dimension {}", id));
    //     }

    //     return std::make_tuple(std::string{name}, length);
    // }


    // std::tuple<std::string, nc_type, std::vector<int>, int> Group::inquire_variable(
    //     int const id) const
    // {
    //     static char name[NC_MAX_NAME];
    //     nc_type data_type;
    //     int nr_dimensions;
    //     static int dimension_ids[NC_MAX_VAR_DIMS];
    //     int nr_attributes;

    //     int const status{
    //         ::nc_inq_var(_id, id, name, &data_type, &nr_dimensions, dimension_ids, &nr_attributes)};

    //     if(status != NC_NOERR)
    //     {
    //         throw std::runtime_error(fmt::format("Cannot inquire about dimension {}", id));
    //     }

    //     return std::make_tuple(
    //             std::string{name}, data_type, std::vector<int>(dimension_ids, dimension_ids + nr_dimensions),
    //             nr_attributes
    //         );
    // }


    // std::vector<int> Group::group_ids() const
    // {
    //     int nr_groups;
    //     static int group_ids[32767];  // TODO According to docs. Which constant to use?

    //     int const status{::nc_inq_grps(_id, &nr_groups, group_ids)};

    //     if(status != NC_NOERR)
    //     {
    //         throw std::runtime_error("Cannot obtain group IDs");
    //     }

    //     return std::vector<int>(group_ids, group_ids + nr_groups);
    // }

}  // namespace lue::data_model::netcdf4
