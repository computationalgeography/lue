#include "lue/data_model/netcdf4/util.hpp"


namespace lue::data_model::netcdf4 {

    std::map<Dimensions, Variables> group_variables(
        Group const& group)
    {
        std::map<Dimensions, Variables> result;

        for(auto const& variable: group.variables())
        {
            result[variable.dimensions()].push_back(variable);
        }

        return result;
    }


    bool is_geographic_coordinate_system(
        Dimensions const& dimensions)
    {
        return dimensions.size() == 2 && dimensions[0].name() == "lat" && dimensions[1].name() == "lon";
    }

}  // namespace lue::data_model::netcdf4
