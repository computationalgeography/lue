#include "lue/cxx_api/time_discretization.h"
#include <cassert>
#include <map>
#include <stdexcept>


namespace lue {
// namespace {
// 
// std::string const group_name = "_lue_time_discretization";
// 
// 
// std::map<TimeDiscretizationType, std::string> const
//     string_by_discretization_type = {
//         { TimeDiscretizationType::regular_grid, "regular_grid" }
// };
// 
// 
// std::map<std::string, TimeDiscretizationType> const
//     discretization_type_by_string = {
//         { "regular_grid", TimeDiscretizationType::regular_grid }
// };
// 
// 
// 
// std::string time_discretization_type_to_string(
//     TimeDiscretizationType const type)
// {
//     auto const it = string_by_discretization_type.find(type);
//     assert(it != string_by_discretization_type.end());
// 
//     return (*it).second;
// }
// 
// 
// TimeDiscretizationType parse_time_discretization_type(
//     std::string const& string)
// {
//     auto const it = discretization_type_by_string.find(string);
// 
//     if(it == discretization_type_by_string.end()) {
//         throw std::runtime_error("Unknown time discretization type: " + string);
//     }
// 
//     return (*it).second;
// }
// 
// }  // Anonymous namespace
// 
// 
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
// TimeDiscretization::TimeDiscretization(
//     hdf5::Identifier const& location)
// 
//     : hdf5::Group(location, group_name),
//       _type(parse_time_discretization_type(
//           attributes().read<std::string>("discretization_type"))),
//       _property(id(), "property")
// 
// {
//     assert(id().is_valid());
// }
// 
// 
// TimeDiscretization::TimeDiscretization(
//     hdf5::Identifier&& location)
// 
//     : hdf5::Group(std::forward<hdf5::Identifier>(location)),
//       _type(parse_time_discretization_type(
//           attributes().read<std::string>("discretization_type"))),
//       _property(id(), "property")
// 
// {
//     assert(id().is_valid());
// }
// 
// 
// /*!
//     @ingroup    lue_cxx_api_group
// */
// TimeDiscretization create_time_discretization(
//     hdf5::Identifier const& location,
//     TimeDiscretizationConfiguration const& configuration)
// {
//     if(time_discretization_exists(location)) {
//         throw std::runtime_error("TimeDiscretization already exists");
//     }
// 
//     auto group = create_group(location, group_name);
// 
//     hdf5::Attributes time_discretization_attributes(group.id());
// 
//     time_discretization_attributes.write<std::string>("discretization_type",
//         time_discretization_type_to_string(configuration.type()));
// 
//     group.create_soft_link(configuration.property(), "property");
// 
//     return TimeDiscretization(group.id());
// }

} // namespace lue
