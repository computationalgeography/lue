#include "lue/data_model/netcdf4/variable.hpp"
#include <fmt/format.h>


namespace lue::data_model::netcdf4 {

    namespace {

        std::tuple<std::string, nc_type, std::vector<Identifier>, int> inquire_variable(
            Identifier const& group_id,
            Identifier const& variable_id)
        {
            static char name[NC_MAX_NAME];
            nc_type data_type;
            int nr_dimensions;
            static int dimension_ids[NC_MAX_VAR_DIMS];
            int nr_attributes;

            int const status{
                    ::nc_inq_var(
                            group_id, variable_id, name, &data_type, &nr_dimensions, dimension_ids,
                            &nr_attributes
                        )
                };

            if(status != NC_NOERR)
            {
                throw std::runtime_error(fmt::format("Cannot inquire about variable"));
            }

            return std::make_tuple(
                    std::string{name}, data_type,
                    std::vector<Identifier>(dimension_ids, dimension_ids + nr_dimensions), nr_attributes
                );
        }

    }  // Anonymous namespace


    Variable::Variable(
        Identifier const& group_id,
        Identifier const& variable_id):

        _id{variable_id}

    {
        Identifiers dimension_ids;
        int nr_attributes;
        std::tie(_name, _data_type, dimension_ids, nr_attributes) = inquire_variable(group_id, _id);

        _attributes.reserve(nr_attributes);
        for(int i = 0; i < nr_attributes; ++i)
        {
            _attributes.push_back(Attribute{group_id, _id, i});
        }

        _dimensions.reserve(dimension_ids.size());
        for(int i: dimension_ids)
        {
            _dimensions.push_back(Dimension{group_id, i});
        }
    }


    std::string const& Variable::name() const
    {
        return _name;
    }


    Attributes const& Variable::attributes() const
    {
        return _attributes;
    }


    Dimensions const& Variable::dimensions() const
    {
        return _dimensions;
    }


    nc_type Variable::data_type() const
    {
        return _data_type;
    }


    bool Variable::is_coordinate_variable() const
    {
        return _dimensions.size() == 1 && _dimensions.front().name() == _name;
    }

}  // namespace lue::data_model::netcdf4
