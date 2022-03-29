#pragma once
#include "lue/data_model/netcdf4/group.hpp"
#include <map>


namespace lue::data_model::netcdf4 {

    std::map<Dimensions, Variables> group_variables(Group const& group);

    bool is_geographic_coordinate_system(Dimensions const& dimensions);

}  // namespace lue::data_model::netcdf4
