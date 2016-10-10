#include "lue/cxx_api/space_discretization.h"
#include <cassert>
#include <map>
#include <stdexcept>


namespace lue {
namespace {

// static std::string const group_name = "_lue_space_discretization";


std::map<SpaceDiscretizationType, std::string> const
    string_by_discretization_type = {
        { SpaceDiscretizationType::cartesian_grid, "cartesian_grid" }
};


std::map<std::string, SpaceDiscretizationType> const
    discretization_type_by_string = {
        { "cartesian_grid", SpaceDiscretizationType::cartesian_grid }
};

}  // Anonymous namespace


std::string space_discretization_type_to_string(
    SpaceDiscretizationType const type)
{
    auto const it = string_by_discretization_type.find(type);
    assert(it != string_by_discretization_type.end());

    return (*it).second;
}


SpaceDiscretizationType parse_space_discretization_type(
    std::string const& string)
{
    auto const it = discretization_type_by_string.find(string);

    if(it == discretization_type_by_string.end()) {
        throw std::runtime_error("Unknown space discretization type: " +
            string);
    }

    return (*it).second;
}


// /*!
//     @ingroup    lue_cxx_api_group
// */
// bool discretization_exists(
//     hdf5::Identifier const& location)
// {
//     return group_exists(location, group_name);
// }
// 
// 
// SpaceDiscretization::SpaceDiscretization(
//     hdf5::Identifier const& location)
// 
//     : hdf5::Group(location, group_name)
// 
// {
//     // assert(property_sets_exists(id()));
//     // _property_sets = std::make_unique<PropertySets>(id());
// }
// 
// 
// SpaceDiscretization::SpaceDiscretization(
//     hdf5::Identifier&& location)
// 
//     : hdf5::Group(std::forward<hdf5::Identifier>(location))
// 
// {
//     assert(id().is_valid());
// 
//     // // Open property sets collection.
//     // assert(property_sets_exists(id()));
//     // _property_sets = std::make_unique<PropertySets>(id());
// }
// 
// 
// /*!
//     @ingroup    lue_cxx_api_group
// */
// SpaceDiscretization create_space_discretization(
//     hdf5::Identifier const& location)
// {
//     if(space_discretization_exists(location)) {
//         throw std::runtime_error("SpaceDiscretization already exists");
//     }
// 
//     auto group = create_group(location, group_name);
// 
//     // TODO add stuff
// 
//     return SpaceDiscretization(group.id());
// }

} // namespace lue
