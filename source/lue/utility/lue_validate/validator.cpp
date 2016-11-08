#include "lue/utility/lue_validate/validator.h"
#include "lue/cxx_api/property_set_configuration.h"
#include "lue/cxx_api/shape.h"
#include "lue/cxx_api/space_domain_configuration.h"
#include "lue/cxx_api/space_discretization.h"
#include "lue/cxx_api/time_domain_configuration.h"
#include "lue/cxx_api/time_discretization.h"
#include "lue/cxx_api/value_configuration.h"
#include <boost/format.hpp>
#include <memory>
// Use the LUE/HDF5 API as much as possible, not the LUE C++ API itself!


namespace lue {
namespace {

// void not_implemented(
//     hdf5::Identifier const& id,
//     std::string const& message,
//     Errors& errors)
// {
//     errors.emplace_back(id, "not implemented yet: " + message);
// }


void validation_not_implemented(
    hdf5::Identifier const& id,
    std::string const& message,
    Errors& errors)
{
    errors.emplace_back(id, "validation not implemented yet: " + message);
}


void missing_group(
    hdf5::Identifier const& id,
    std::string const& name,
    Errors& errors)
{
    errors.emplace_back(id, "group '" + name + "' does not exist");
}


void missing_dataset(
    hdf5::Identifier const& id,
    std::string const& name,
    Errors& errors)
{
    errors.emplace_back(id, "dataset '" + name + "' does not exist");
}


void missing_attribute(
    hdf5::Identifier const& id,
    std::string const& name,
    Errors& errors)
{
    errors.emplace_back(id, "attribute '" + name + "' does not exist");
}


void missing_soft_link(
    hdf5::Identifier const& id,
    std::string const& name,
    Errors& errors)
{
    errors.emplace_back(id, "soft link '" + name + "' does not exist");
}


void link_does_not_point_to_group(
    hdf5::Group const& group,
    std::string const& name,
    hdf5::Group const& target,
    Errors& errors)
{
    errors.emplace_back(group.id(), "soft link '" + name +
        "' does not point to " + target.id().pathname());
}


void invalid_mobility(
    hdf5::Group const& group,
    SpaceDomainType const domain_type,
    Mobility const mobility,
    Errors& errors)
{
    errors.emplace_back(group.id(), boost::str(boost::format(
        "space domain type '%1%' cannot be combined with mobility '%2%'")
        % space_domain_type_to_string(domain_type)
        % mobility_to_string(mobility)));
}


void invalid_domain_item_type(
    hdf5::Group const& group,
    SpaceDomainType const domain_type,
    SpaceDomainItemType const domain_item_type,
    Errors& errors)
{
    errors.emplace_back(group.id(), boost::str(boost::format(
        "space domain type '%1%' cannot be combined with "
        "domain item type '%2%'")
        % space_domain_type_to_string(domain_type)
        % space_domain_item_type_to_string(domain_item_type)));
}


void invalid_domain_item_type(
    hdf5::Group const& group,
    TimeDomainType const domain_type,
    TimeDomainItemType const domain_item_type,
    Errors& errors)
{
    errors.emplace_back(group.id(), boost::str(boost::format(
        "time domain type '%1%' cannot be combined with "
        "domain item type '%2%'")
        % time_domain_type_to_string(domain_type)
        % time_domain_item_type_to_string(domain_item_type)));
}


void invalid_shape_through_time(
    hdf5::Group const& group,
    TimeDomainType const time_domain_type,
    ShapeThroughTime const shape_through_time,
    Errors& errors)
{
    errors.emplace_back(group.id(), boost::str(boost::format(
        "time domain type '%1%' cannot be combined with "
        "space through time '%2%'")
        % time_domain_type_to_string(time_domain_type)
        % shape_through_time_to_string(shape_through_time)));
}


void assert_groups_exist(
    hdf5::Group const& group,
    std::vector<std::string> const& names,
    Errors& errors)
{
    for(auto const& name: names) {
        if(!group.contains_group(name)) {
            missing_group(group.id(), name, errors);
        }
    }
}


void assert_datasets_exist(
    hdf5::Group const& group,
    std::vector<std::string> const& names,
    Errors& errors)
{
    for(auto const& name: names) {
        if(!group.contains_dataset(name)) {
            missing_dataset(group.id(), name, errors);
        }
    }
}


void assert_shape_equals(
    hdf5::Dataset const& dataset,
    hdf5::Dataspace const& dataspace,
    Shape const& shape,
    Errors& errors)
{
    auto const extents = dataspace.dimension_extents();

    for(size_t d = 0; d < extents.size(); ++d) {
        // If a dimension in shape's extent is 0, that means that the
        // extent can have any size.
        if(shape[d] != 0 && extents[d] != shape[d]) {
            errors.emplace_back(dataset.id(), boost::str(boost::format(
                "extent of dimension '%1%' is '%2%', but must be '%3%'")
                % d
                % extents[d]
                % shape[d]));
        }
    }
}


// void assert_shape_equals(
//     hdf5::Dataset const& dataset,
//     Shape const& shape,
//     Errors& errors)
// {
//     assert_shape_equals(dataset, dataset.dataspace(), shape, errors);
// }


void assert_rank_equals(
    hdf5::Dataset const& dataset,
    hdf5::Dataspace const& dataspace,
    size_t const rank,
    Errors& errors)
{
    if(static_cast<Shape::size_type>(dataspace.nr_dimensions()) != rank) {
        errors.emplace_back(dataset.id(), boost::str(boost::format(
            "rank is '%1%', but must be '%2%'")
            % dataspace.nr_dimensions()
            % rank));
    }
}


void assert_rank_equals(
    hdf5::Dataset const& dataset,
    size_t const rank,
    Errors& errors)
{
    assert_rank_equals(dataset, dataset.dataspace(), rank, errors);
}


void assert_dataspace_equals(
    hdf5::Dataset const& dataset,
    Shape const& shape,
    Errors& errors)
{
    auto const dataspace = dataset.dataspace();

    assert_rank_equals(dataset, dataspace, shape.size(), errors);

    if(static_cast<Shape::size_type>(dataspace.nr_dimensions()) ==
            shape.size()) {
        assert_shape_equals(dataset, dataspace, shape, errors);
    }
}


void assert_dataset_has_datatype(
    hdf5::Dataset const& dataset,
    hid_t const datatype,
    Errors& errors)
{
    auto const dataset_datatype = dataset.type_id();

    if(!hdf5::datatypes_are_equal(dataset_datatype, datatype)) {
        errors.emplace_back(dataset.id(), boost::str(boost::format(
            "datatype of dataset is '%1%', but must be '%2%'")
            % hdf5::standard_datatype_as_string(dataset_datatype)
            % hdf5::standard_datatype_as_string(datatype)));
    }
}


void assert_attribute_has_datatype(
    hdf5::Group const& group,
    std::string const& name,
    hid_t const datatype,
    Errors& errors)
{
    auto const attribute_datatype = group.attributes().type_id(name);

    if(!hdf5::datatypes_are_equal(attribute_datatype, datatype)) {
        errors.emplace_back(group.id(), boost::str(boost::format(
            "datatype of attribute '%1%' is '%2%', but must be '%3%'")
            % name
            % hdf5::standard_datatype_as_string(attribute_datatype)
            % hdf5::standard_datatype_as_string(datatype)));
    }
}


void assert_attributes_exist(
    hdf5::Group const& group,
    std::vector<std::string> const& names,
    Errors& errors)
{
    for(auto const& name: names) {
        if(!group.contains_attribute(name)) {
            missing_attribute(group.id(), name, errors);
        }
    }
}


void assert_soft_links_exist(
    hdf5::Group const& group,
    std::vector<std::string> const& names,
    Errors& errors)
{
    for(auto const& name: names) {
        if(!group.contains_soft_link(name)) {
            missing_soft_link(group.id(), name, errors);
        }
    }
}


void assert_link_points_to(
    hdf5::Group const& group,
    std::string const& name,
    hdf5::Group const& target,
    Errors& errors)
{
    if(hdf5::open_group(group, name) != target) {
        link_does_not_point_to_group(group, name, target, errors);
    }
}


// // void validate_domains(
// //     hdf5::Group const& group)
// // {
// //     // Here we can assume that the time and space domains themselves are
// //     // valid. We must check whether the *combination* of the time and
// //     // space domains is valid.
// // 
// //     auto const time_domain = hdf5::open_group(group, "lue_time");
// //     auto const space_domain = hdf5::open_group(group, "lue_space");
// // 
// //     auto const time_domain_type = parse_time_domain_type(
// //         time_domain.attribute<std::string>("domain_type"));
// //     // auto const time_domain_item_type = parse_time_domain_item_type(
// //     //     time_domain.attribute<std::string>("domain_item_type"));
// // 
// //     // auto const space_domain_type = parse_space_domain_type(
// //     //     space_domain.attribute<std::string>("domain_type"));
// //     auto const mobility = parse_mobility(
// //         space_domain.attribute<std::string>("mobility"));
// //     // auto const space_domain_item_type = parse_space_domain_item_type(
// //     //     space_domain.attribute<std::string>("domain_item_type"));
// // 
// // 
// //     // time_domain_type:
// //     // - omnipresent
// //     // - shared_constant_collection
// // 
// //     // time_domain_item_type:
// //     // - none
// //     // - period
// // 
// //     // space_domain_type:
// //     // - omnipresent
// //     // - located
// // 
// //     // mobility:
// //     // - stationary
// //     // - mobile
// // 
// //     // space_domain_item_type:
// //     // - none
// //     // - box
// // 
// // 
// //     // Illegal combinations:
// // 
// //     // omnipresent time domain + mobile space items
// //     if(time_domain_type == TimeDomainType::omnipresent &&
// //             mobility == Mobility::mobile) {
// //         errors.emplace_back(group.id(),
// //             "mobility requires a located time domain");
// //     }
// // }
// // 
// // 
// // void validate_space_domain(
// //     hdf5::Group const& group,
// //     SpaceDomainType const domain_type,
// //     Mobility const mobility,
// //     SpaceDomainItemType const domain_item_type)
// // {
// //     // space_domain_type:
// //     // - omnipresent
// //     // - located
// // 
// //     // mobility:
// //     // - stationary
// //     // - mobile
// // 
// //     // space_domain_item_type:
// //     // - none
// //     // - box
// // 
// // 
// //     // Illegal combinations:
// //     // - omnipresent + mobile
// //     // - omnipresent + not-none
// //     // - located + none
// // 
// // 
// //     switch(domain_type) {
// //         case SpaceDomainType::omnipresent: {
// //             switch(mobility) {
// //                 case Mobility::stationary: {
// //                     switch(domain_item_type) {
// //                         case SpaceDomainItemType::none: {
// //                             // Nothing to validate.
// //                             break;
// //                         }
// //                         default: {
// //                             invalid_domain_item_type(group, domain_type,
// //                                 domain_item_type);
// //                             break;
// //                         }
// //                     }
// // 
// //                     break;
// //                 }
// //                 case Mobility::mobile: {
// //                     invalid_mobility(group, domain_type, mobility);
// // 
// //                     switch(domain_item_type) {
// //                         case SpaceDomainItemType::none: {
// //                             // TODO
// //                             not_implemented("omnipresent/mobile/none");
// //                             break;
// //                         }
// //                         case SpaceDomainItemType::box: {
// //                             // TODO
// //                             not_implemented("omnipresent/mobile/box");
// //                             break;
// //                         }
// //                     }
// // 
// //                     break;
// //                 }
// //             }
// // 
// //             break;
// //         }
// //         case SpaceDomainType::located: {
// //             switch(mobility) {
// //                 case Mobility::stationary: {
// //                     switch(domain_item_type) {
// //                         case SpaceDomainItemType::none: {
// //                             invalid_domain_item_type(group, domain_type,
// //                                 domain_item_type);
// //                             break;
// //                         }
// //                         case SpaceDomainItemType::box: {
// //                             // TODO
// // 
// // std::cout << "located/stationary/box\n";
// // 
// // 
// // 
// //                             break;
// //                         }
// //                     }
// // 
// //                     break;
// //                 }
// //                 case Mobility::mobile: {
// //                     switch(domain_item_type) {
// //                         case SpaceDomainItemType::none: {
// //                             invalid_domain_item_type(group, domain_type,
// //                                 domain_item_type);
// //                             break;
// //                         }
// //                         case SpaceDomainItemType::box: {
// //                             // TODO
// // 
// //                             not_implemented("located/mobile/box");
// //                             break;
// //                         }
// //                     }
// // 
// //                     break;
// //                 }
// //             }
// // 
// //             break;
// //         }
// //     }
// // }
// // 
// // 
// // void validate_space_domain(
// //     hdf5::Group const& group)
// // {
// //     assert_attributes_exist(group, {
// //         "domain_type",
// //         "domain_item_type",
// //         "mobility"
// //     });
// // 
// //     try {
// //         if(group.contains_attribute("domain_type")) {
// //             auto const domain_type_string = group.attribute<std::string>(
// //                 "domain_type");
// //             auto const domain_type = parse_space_domain_type(
// //                 domain_type_string);
// // 
// //             if(group.contains_attribute("mobility")) {
// //                 auto const mobility_string = group.attribute<std::string>(
// //                     "mobility");
// //                 auto const mobility = parse_mobility(mobility_string);
// // 
// //                 if(group.contains_attribute("domain_type")) {
// //                     auto const domain_item_type_string =
// //                         group.attribute<std::string>("domain_item_type");
// //                     auto const domain_item_type =
// //                         parse_space_domain_item_type(domain_item_type_string);
// // 
// //                     validate_space_domain(group, domain_type, mobility,
// //                         domain_item_type);
// //                 }
// //             }
// //         }
// //     }
// //     catch(std::exception const& exception) {
// //         errors.emplace_back(group.id(), exception.what());
// //     }
// // }
// // 
// // 
// // void validate_sc_periods(
// //     hdf5::Group const& group)
// // {
// //     assert_attributes_exist(group, {
// //         "resolution"
// //     });
// // 
// //     // TODO
// // 
// //     // try {
// //     //     if(group.contains_attribute("resolution") {
// //     //         auto const resolution_string =
// //     //             group.attribute<std::string>("resolution");
// //     //         auto const resolution =
// //     //             parse_time_domain_item_type(
// //     //                 resolution_string);
// // 
// //     //         validate_time_domain(group, domain_type, resolution);
// // 
// //     //     }
// //     // }
// //     // catch(std::exception const& exception) {
// //     //     errors.emplace_back(group.id(), exception.what());
// //     // }
// // }
// // 
// // 
// // void validate_time_domain(
// //     hdf5::Group const& group,
// //     TimeDomainType const domain_type,
// //     TimeDomainItemType const domain_item_type)
// // {
// //     // time_domain_type:
// //     // - omnipresent
// //     // - shared_constant_collection
// // 
// //     // time_domain_item_type:
// //     // - none
// //     // - period
// // 
// // 
// //     // Illegal combinations:
// //     // - omnipresent + not-none
// //     // - shared_constant_collection + none
// // 
// // 
// //     switch(domain_type) {
// //         case TimeDomainType::omnipresent: {
// //             switch(domain_item_type) {
// //                 case TimeDomainItemType::none: {
// //                     // Nothing to validate.
// //                     break;
// //                 }
// //                 case TimeDomainItemType::period: {
// //                     invalid_domain_item_type(group, domain_type,
// //                         domain_item_type);
// //                     break;
// //                 }
// //             }
// // 
// //             break;
// //         }
// //         case TimeDomainType::shared_constant_collection: {
// //             switch(domain_item_type) {
// //                 case TimeDomainItemType::none: {
// //                     invalid_domain_item_type(group, domain_type,
// //                         domain_item_type);
// //                     break;
// //                 }
// //                 case TimeDomainItemType::period: {
// //                     validate_sc_periods(group);
// //                     break;
// //                 }
// //             }
// // 
// //             break;
// //         }
// //     }
// // }
// // 
// // 
// // void validate_time_domain(
// //     hdf5::Group const& group)
// // {
// //     assert_attributes_exist(group, {
// //         "domain_type",
// //         "domain_item_type"
// //     });
// // 
// // 
// //     try {
// //         if(group.contains_attribute("domain_type")) {
// //             auto const domain_type_string = group.attribute<std::string>(
// //                 "domain_type");
// //             auto const domain_type = parse_time_domain_type(
// //                 domain_type_string);
// // 
// //             if(group.contains_attribute("domain_type")) {
// //                 auto const domain_item_type_string =
// //                     group.attribute<std::string>("domain_item_type");
// //                 auto const domain_item_type = parse_time_domain_item_type(
// //                     domain_item_type_string);
// // 
// //                 validate_time_domain(group, domain_type, domain_item_type);
// //             }
// //         }
// //     }
// //     catch(std::exception const& exception) {
// //         errors.emplace_back(group.id(), exception.what());
// //     }
// // }
// // 
// // 
// // void validate_domain(
// //     hdf5::Group const& group)
// // {
// //     // Assume there are no properties. Only test the domain itself.
// // 
// //     // Each domain must contain information about the time and space
// //     // domains.
// // 
// //     size_t const nr_errors = errors.size();
// // 
// //     assert_groups_exist(group, {
// //         "lue_time",
// //         "lue_space"
// //     });
// // 
// //     if(group.contains_group("lue_time")) {
// //         validate_time_domain(hdf5::open_group(group, "lue_time"));
// //     }
// // 
// //     if(group.contains_group("lue_space")) {
// //         validate_space_domain(hdf5::open_group(group, "lue_space"));
// //     }
// // 
// //     bool const domains_are_valid = nr_errors == errors.size();
// // 
// // 
// //     // Iff time and space domains are valid, validate whether the combination
// //     // is valid.
// //     if(domains_are_valid) {
// //         validate_domains(group);
// //     }
// // }
// // 
// // 
// // void validate_property(
// //     hdf5::Group const& group)
// // {
// //     assert_soft_links_exist(group, {
// //         "lue_domain"
// //     });
// // 
// //     // validate_properties verifies that the property domain equals
// //     // the property set domain. It also calls validate_domain on the
// //     // property set domain. So we don't have to do that here.
// // 
// //     // Test the domain and the property are a valid combination.
// //     // Assume that the time and space domain themselves are validated
// //     // elsewhere.
// //     not_implemented("domain + property combination");
// // }
// 
// 
// void o_box_validate_properties(
//     hdf5::Group const& domain,
//     hdf5::Group const& group,
//     Errors& errors)
// {
//     // omnipresent/located/stationary/box
//     // Validate layout of the items.
// 
// 
//     // Time domain
//     // Empty
// 
// 
//     // Space domain
//     // Per item a box
//     // Dataset 'coordinates'
//     // Shape: 2D:
//     //     shape[0]: extent represents items
//     //     shape[1]: 2 * space dimensionality numbers representing 2 corners
//     //         of the box
//     // TODO space/rank attribute
//     auto const space_domain = hdf5::open_group(domain, "lue_space");
// 
//     assert_datasets_exist(space_domain, {
//         "coordinates"
//     });
// 
//     if(space_domain.contains_dataset("coordinates")) {
//         const int rank = 2;
// 
//         auto const dataset = hdf5::open_dataset(space_domain,
//             "coordinates");
//         auto const dataspace = dataset.dataspace();
// 
//         assert_rank_equals(dataset, 2);
//         assert_shape_equals(dataset, Shape{0, 2 * rank});
//         assert_dataset_has_datatype(dataset, H5T_IEEE_F64LE);
//     }
// 
// 
//     // Items
//     // - Id collection in domain/ids dataset
//     // - dataset
//     //     - shape: (unlimited)
//     //     - type: uint64_t
//     assert_datasets_exist(domain, {
//         "ids"
//     });
// 
// 
//     if(domain.contains_dataset("ids")) {
//         auto const dataset = open_dataset(domain, "ids");
//         assert_dataspace_equals(dataset, Shape{0});
//         assert_dataset_has_datatype(dataset, H5T_STD_U64LE);
//     }
// 
// 
//     auto const group_names = hdf5::group_names(group);
// 
//     for(auto const& name: group_names) {
//         auto const property = hdf5::open_group(group, name);
// 
//         o_validate_property(property);
//     }
// }
// 
// 
// void sc_p_s_box_validate_properties(
//     hdf5::Group const& domain,
//     hdf5::Group const& group,
//     Errors& errors)
// {
//     // shared_constant/period/located/stationary/box
//     // Validate layout of the items.
// 
//     try {
//         // Time domain
//         // - Resolution attribute
//         // - 2D dataset
//         //     - first dimension is time domain item dimension
//         //     - extent of second dimension depends on resolution:
//         //         - month: 3: year, month, nr_months
//         auto const time_domain = hdf5::open_group(domain, "lue_time");
// 
//         assert_datasets_exist(time_domain, {
//             "coordinates"
//         });
// 
// 
//         if(time_domain.contains_dataset("coordinates")) {
//             auto const dataset = hdf5::open_dataset(time_domain,
//                 "coordinates");
//             auto const dataspace = dataset.dataspace();
// 
//             assert_rank_equals(dataset, 2);
// 
//             assert_attributes_exist(time_domain, {
//                 "lue_resolution"
//             });
// 
//             // if(time_domain.contains_attribute("resolution")) {
//             //     auto const time_resolution = parse_time_resolution(
//             //         time_domain.attribute<std::string>("resolution"));
// 
//             //     if(time_resolution = TimeResolution::Month) {
//             //             auto const extents = dataspace.dimension_extents();
// 
// 
//             //     }
//             //     else {
//             //         not_implemented("time resolution " +
//             //             time_resolution_as_string(time_resolution));
//             //     }
//             // }
//         }
// 
// 
//         // Space domain
// 
// 
//         // Items
// 
// 
//         // Property
// 
// 
// 
// 
//     }
//     catch(std::exception const& exception) {
//         errors.emplace_back(domain.id(), exception.what());
//     }
// }
// 
// 
// void sc_p_validate_properties(
//     hdf5::Group const& domain,
//     hdf5::Group const& group,
//     SpaceDomainType const space_domain_type,
//     Mobility const mobility,
//     SpaceDomainItemType const space_domain_item_type,
//     Errors& errors)
// {
//     auto const space_domain = hdf5::open_group(domain, "lue_space");
// 
//     switch(space_domain_type) {
//         case SpaceDomainType::omnipresent: {
//             switch(mobility) {
//                 case Mobility::stationary: {
//                     switch(space_domain_item_type) {
//                         case SpaceDomainItemType::none: {
//                             // TODO sc_p_none_validate_properties(domain,
//                             //     group);
//                             not_implemented("shared_constant/period/"
//                                 "omnipresent/stationary/none");
//                             break;
//                         }
//                         default: {
//                             invalid_domain_item_type(space_domain,
//                                 space_domain_type, space_domain_item_type);
//                             break;
//                         }
//                     }
// 
//                     break;
//                 }
//                 case Mobility::mobile: {
//                     invalid_mobility(space_domain, space_domain_type,
//                         mobility);
//                     break;
//                 }
//             }
// 
//             break;
//         }
//         case SpaceDomainType::located: {
//             switch(mobility) {
//                 case Mobility::stationary: {
//                     switch(space_domain_item_type) {
//                         case SpaceDomainItemType::none: {
//                             invalid_domain_item_type(space_domain,
//                                 space_domain_type, space_domain_item_type);
//                             break;
//                         }
//                         case SpaceDomainItemType::box: {
//                             sc_p_s_box_validate_properties(domain, group);
//                             break;
//                         }
//                     }
// 
//                     break;
//                 }
//                 case Mobility::mobile: {
//                     switch(space_domain_item_type) {
//                         case SpaceDomainItemType::none: {
//                             invalid_domain_item_type(space_domain,
//                                 space_domain_type, space_domain_item_type);
//                             break;
//                         }
//                         case SpaceDomainItemType::box: {
//                             // TODO sc_p_m_box_validate_properties(domain,
//                             //     group);
//                             not_implemented("shared_constant/period/"
//                                 "located/mobile/box");
//                             break;
//                         }
//                     }
// 
//                     break;
//                 }
//             }
// 
//             break;
//         }
//     }
// }




std::unique_ptr<ValueConfiguration> validate_value_configuration(
    hdf5::Group const& value,
    Errors& errors)
{
    std::unique_ptr<ValueConfiguration> configuration;

    assert_attributes_exist(value, {
            "lue_shape_through_time",
            "lue_shape_per_item"
        }, errors);


    if(value.contains_attribute("lue_shape_through_time") &&
        value.contains_attribute("lue_shape_per_item")) {

        try {
            auto const shape_through_time = parse_shape_through_time(
                value.attribute<std::string>("lue_shape_through_time"));
            auto const shape_per_item = parse_shape_per_item(
                value.attribute<std::string>("lue_shape_per_item"));

            configuration = std::make_unique<ValueConfiguration>(
                shape_through_time, shape_per_item);
        }
        catch(std::exception const& exception) {
            errors.emplace_back(value.id(), exception.what());
        }
    }

    return configuration;
}


namespace constant_size {
namespace time {
namespace omnipresent {

std::vector<item_t> validate_items(
    hdf5::Group const& domain,
    Errors& errors)
{
    // Items
    // - Id collection in domain/ids dataset
    // - dataset
    //     - shape: (unlimited)
    //     - type: uint64_t
    assert_datasets_exist(domain, {
            "ids"
        }, errors);


    std::vector<item_t> item_ids;

    if(domain.contains_dataset("ids")) {
        auto const dataset = open_dataset(domain, "ids");
        auto const nr_errors = errors.size();

        assert_dataspace_equals(dataset, Shape{0}, errors);
        assert_dataset_has_datatype(dataset, H5T_STD_U64LE, errors);

        if(errors.size() == nr_errors) {
            auto const dataspace = dataset.dataspace();
            auto const extents = dataspace.dimension_extents();
            auto const nr_items = extents[0];

            std::vector<hsize_t> start{0};
            std::vector<hsize_t> count{nr_items};
            std::vector<hsize_t> stride{1};
            item_ids.resize(nr_items);

            dataset.read(H5T_STD_U64LE, start, count, stride, item_ids.data());
        }
    }

    return item_ids;
}


void validate_omnipresent_space_domain(
    hdf5::Group const& /* space_domain */,
    std::vector<item_t> const /* item_ids */,
    Errors& /* errors */)
{
    // Nothing to do.
}


void validate_stationary_box_space_domain(
    hdf5::Group const& space_domain,
    std::vector<item_t> const item_ids,
    Errors& errors)
{
    // A box per item.
    // - Number of boxes must equal the number of items.
    // - Box consists of two corner points.
    assert_datasets_exist(space_domain, {
            "coordinates"
        }, errors);


    if(space_domain.contains_dataset("coordinates")) {
        // dataset with:
        // - rank == 2
        // - value_type == float64
        // - shape == (nr_items, 4)
        auto const dataset = open_dataset(space_domain, "coordinates");

        hsize_t const nr_items = item_ids.size();
        assert_dataspace_equals(dataset, Shape{nr_items, 4}, errors);
        assert_dataset_has_datatype(dataset, H5T_IEEE_F64LE, errors);
    }
}


void validate_space_domain(
    hdf5::Group const& space_domain,
    SpaceDomainConfiguration const& space_domain_configuration,
    std::vector<item_t> const item_ids,
    Errors& errors)
{
    // Space domain contains space domain items for all phenomenon items.
    // There is no variation through time.

    switch(space_domain_configuration.type()) {
        case SpaceDomainType::omnipresent: {
            switch(space_domain_configuration.mobility()) {
                case Mobility::stationary: {
                    switch(space_domain_configuration.item_type()) {
                        case SpaceDomainItemType::none: {
                            validate_omnipresent_space_domain(
                                space_domain, item_ids, errors);
                            break;
                        }
                        default: {
                            invalid_domain_item_type(space_domain,
                                space_domain_configuration.type(),
                                space_domain_configuration.item_type(),
                                errors);
                            break;
                        }
                    }

                    break;
                }
                case Mobility::mobile: {
                    invalid_mobility(space_domain,
                        space_domain_configuration.type(),
                        space_domain_configuration.mobility(), errors);
                    break;
                }
            }

            break;
        }
        case SpaceDomainType::located: {
            switch(space_domain_configuration.mobility()) {
                case Mobility::stationary: {
                    switch(space_domain_configuration.item_type()) {
                        case SpaceDomainItemType::none: {
                            invalid_domain_item_type(space_domain,
                                space_domain_configuration.type(),
                                space_domain_configuration.item_type(),
                                errors);
                            break;
                        }
                        case SpaceDomainItemType::box: {
                            validate_stationary_box_space_domain(space_domain,
                                item_ids, errors);
                            break;
                        }
                    }

                    break;
                }
                case Mobility::mobile: {
                    invalid_mobility(space_domain,
                        space_domain_configuration.type(),
                        space_domain_configuration.mobility(), errors);
                    break;
                }
            }

            break;
        }
    }
}


void validate_constant_same_shape_value(
    hdf5::Group const& property,
    hdf5::Group const& value,
    std::vector<item_t> const item_ids,
    Errors& errors)
{
    assert_datasets_exist(value, {
            "value"
        }, errors);


    if(value.contains_dataset("value")) {
        auto const dataset = hdf5::open_dataset(value, "value");

        // Shape of dataset must be at least 2D. First dimension is
        // for items, subsequent dimensions are for values.

        auto const dataspace = dataset.dataspace();

        if(dataspace.nr_dimensions() <= 1) {
            errors.emplace_back(dataset.id(), boost::str(boost::format(
                "number of dimensions is '%1%', "
                "but must be at least '%2%'")
                % dataspace.nr_dimensions()
                % 2));
        }
        else {
            // Number of items in dataset must equal number of phenomenon
            // items. We don't need to compare item ids, since there are
            // not item ids in the dataset. The order of the values along
            // the first dimension in the dataset corresponds with the
            // order of the item ids in their own dataset.
            auto const extents = dataspace.dimension_extents();

            if(extents[0] != item_ids.size()) {
                errors.emplace_back(dataset.id(), boost::str(
                    boost::format(
                        "extent of dimension '%1%' is '%2%', "
                        "but must be '%3%'")
                    % 0
                    % extents[0]
                    % item_ids.size()));
            }
        }
    }


    if(property.contains_soft_link("lue_space_discretization")) {
        // Value is discretized.

        validation_not_implemented(property.id(),
            "omnipresent/constant/same_shape/space discretization", errors);

        // auto const discretization = hdf5::open_group(value,
        //     "lue_space_discretization");
    }
}


void validate_constant_different_shape_value(
    hdf5::Group const& property,
    hdf5::Group const& value,
    std::vector<item_t> const item_ids,
    Errors& errors)
{
    assert_groups_exist(value, {
            "value"
        }, errors);


    auto const nr_errors = errors.size();

    if(value.contains_group("value")) {
        // Per item a value. All values have:
        // - Same rank
        // - Different shape

        auto const value_ = hdf5::open_group(value, "value");

        assert_attributes_exist(value_, {
                "lue_datatype",
                "lue_nr_items",
                "lue_rank"
            }, errors);


        // nr_items
        if(value_.contains_attribute("lue_nr_items")) {
            assert_attribute_has_datatype(value_, "lue_nr_items",
                H5T_STD_U64LE, errors);

            try {
                auto const nr_items = value_.attribute<uint64_t>(
                    "lue_nr_items");

                // Verify the number value items equals the number of
                // phenomenon items.
                if(nr_items != item_ids.size()) {
                    errors.emplace_back(value_.id(), boost::str(boost::format(
                        "number of items is '%1%', "
                        "but must be '%2%'")
                        % nr_items
                        % item_ids.size()));
                }
            }
            catch(std::exception const& exception) {
                errors.emplace_back(value_.id(), exception.what());
            }
        }


        // rank
        if(value_.contains_attribute("lue_rank")) {
            assert_attribute_has_datatype(value_, "lue_rank",
                H5T_STD_U16LE, errors);

            try {
                // Iterate over all items and verify that for each of them
                // there is a dataset available with the correct rank.

                auto const rank = value_.attribute<uint16_t>("lue_rank");

                for(auto const item_id: item_ids) {
                    auto const dataset_name = std::to_string(item_id);
                    assert_datasets_exist(value_, {
                            dataset_name
                        }, errors);
                    auto const dataset = hdf5::open_dataset(value_,
                        dataset_name);
                    assert_rank_equals(dataset, rank, errors);
                }
            }
            catch(std::exception const& exception) {
                errors.emplace_back(value_.id(), exception.what());
            }
        }
    }


    if(errors.size() == nr_errors) {
        if(property.contains_soft_link("lue_space_discretization")) {
            // Value is discretized.
            // There is no variation through time.
            // Each value is an nD array. The property pointed to by the
            // space discretization link is a property containing the
            // discretization value. For each space domain item, the
            // discretization must have a phenomenon item. This discretization
            // item must provide the information for desciphering the
            // phenomenon item value.

            // Only make sure that the information required for
            // discretizing the value is present. The discretization
            // property itself is fully validated elsewhere.

            auto const discretization_property = hdf5::open_group(property,
                "lue_space_discretization");

            if(!discretization_property.contains_group("lue_domain")) {
                goto stop_validating;
            }


            assert_attributes_exist(discretization_property, {
                    "lue_discretization_type"
                }, errors);

            if(!discretization_property.contains_attribute(
                    "lue_discretization_type")) {
                goto stop_validating;
            }

            auto const discretization_type = parse_space_discretization_type(
                discretization_property.attribute<std::string>(
                    "lue_discretization_type"));

            if(discretization_type != SpaceDiscretizationType::cartesian_grid) {
                validation_not_implemented(property.id(),
                    "space discretization type " +
                        space_discretization_type_to_string(
                            SpaceDiscretizationType::cartesian_grid),
                    errors);
            }


            auto const discretization_domain = hdf5::open_group(
                discretization_property, "lue_domain");


            // - time domain must be omnipresent
            if(!discretization_domain.contains_group("lue_time")) {
                goto stop_validating;
            }

            auto const discretization_time_domain = hdf5::open_group(
                discretization_domain, "lue_time");

            if(!discretization_time_domain.contains_attribute("domain_type")) {
                goto stop_validating;
            }

            auto const discretization_time_domain_type = parse_time_domain_type(
                discretization_time_domain.attribute<std::string>(
                    "domain_type"));

            if(discretization_time_domain_type != TimeDomainType::omnipresent) {
                errors.emplace_back(property.id(), boost::str(boost::format(
                    "space discretization's time domain type is '%1%', "
                    "but must be '%2%'")
                    % time_domain_type_to_string(discretization_time_domain_type)
                    % time_domain_type_to_string(TimeDomainType::omnipresent)));
            }


            // - space domain must be omnipresent
            if(!discretization_domain.contains_group("lue_space")) {
                goto stop_validating;
            }

            auto const discretization_space_domain = hdf5::open_group(
                discretization_domain, "lue_space");

            if(!discretization_space_domain.contains_attribute("domain_type")) {
                goto stop_validating;
            }

            auto const discretization_space_domain_type =
                parse_space_domain_type(
                    discretization_space_domain.attribute<std::string>(
                    "domain_type"));

            if(discretization_space_domain_type !=
                    SpaceDomainType::omnipresent) {
                errors.emplace_back(property.id(), boost::str(boost::format(
                    "space discretization's space domain type is '%1%', "
                    "but must be '%2%'")
                    % space_domain_type_to_string(
                        discretization_space_domain_type)
                    % space_domain_type_to_string(
                        SpaceDomainType::omnipresent)));
            }


            // - nr_items must equal the number of space domain items
            validation_not_implemented(property.id(),
                "space discretization, nr_items", errors);


            // - rank of each value must be less or equal to rank of
            //   phenomenon item
            validation_not_implemented(property.id(),
                "space discretization, rank", errors);

        }
        stop_validating: ;
    }
}


void validate_property(
    hdf5::Group const& property,
    std::vector<item_t> const item_ids,
    Errors& errors)
{
    auto const domain = hdf5::open_group(property, "lue_domain");

    assert_groups_exist(property, {
            "value"
        }, errors);


    if(property.contains_group("value")) {
        auto const value = hdf5::open_group(property, "value");
        auto value_configuration = validate_value_configuration(value,
            errors);

        switch(value_configuration->shape_through_time()) {
            case ShapeThroughTime::constant: {
                switch(value_configuration->shape_per_item()) {
                    case ShapePerItem::same: {
                        validate_constant_same_shape_value(property,
                            value, item_ids, errors);
                        break;
                    }
                    case ShapePerItem::different: {
                        validate_constant_different_shape_value(property,
                            value, item_ids, errors);
                        break;
                    }
                }

                break;
            }
            case ShapeThroughTime::variable: {
                invalid_shape_through_time(value,
                    TimeDomainType::omnipresent,
                    value_configuration->shape_through_time(), errors);
                break;
            }
        }
    }
}


void validate_properties(
    hdf5::Group const& domain,
    hdf5::Group const& properties,
    SpaceDomainConfiguration const& space_domain_configuration,
    Errors& errors)
{
    auto const item_ids = validate_items(domain, errors);
    validate_space_domain(hdf5::open_group(domain, "lue_space"),
        space_domain_configuration, item_ids, errors);


    // Property
    // - nD value per item
    // - not discretized
    auto const property_names = hdf5::group_names(properties);

    for(auto const& property_name: property_names) {
        auto const property = hdf5::open_group(properties, property_name);
        validate_property(property, item_ids, errors);
    }
}

}  // namespace omnipresent


namespace shared {

void validate_items(
    hdf5::Group const& domain,
    Errors& errors)
{
    validation_not_implemented(domain.id(),
        "shared/items", errors);
}


void validate_space_domain(
    hdf5::Group const& space_domain,
    SpaceDomainConfiguration const& /* space_domain_configuration */,
    Errors& errors)
{
    validation_not_implemented(space_domain.id(),
        "shared/space domain", errors);
}


void validate_properties(
    hdf5::Group const& domain,
    hdf5::Group const& /* properties */,
    SpaceDomainConfiguration const& space_domain_configuration,
    Errors& errors)
{
    validate_items(domain, errors);
    validate_space_domain(hdf5::open_group(domain, "lue_space"),
        space_domain_configuration, errors);
}

}  // namespace shared
}  // namespace time


void validate_properties(
    hdf5::Group const& domain,
    hdf5::Group const& properties,
    TimeDomainConfiguration const& time_domain_configuration,
    SpaceDomainConfiguration const& space_domain_configuration,
    Errors& errors)
{
    auto const time_domain = hdf5::open_group(domain, "lue_time");

    switch(time_domain_configuration.type()) {
        case TimeDomainType::omnipresent: {
            switch(time_domain_configuration.item_type()) {
                case TimeDomainItemType::none: {
                    time::omnipresent::validate_properties(domain,
                        properties, space_domain_configuration, errors);
                    break;
                }
                default: {
                    invalid_domain_item_type(time_domain,
                        time_domain_configuration.type(),
                        time_domain_configuration.item_type(), errors);
                    break;
                }
            }

            break;
        }
        case TimeDomainType::shared: {
            switch(time_domain_configuration.item_type()) {
                case TimeDomainItemType::none: {
                    invalid_domain_item_type(time_domain,
                        time_domain_configuration.type(),
                        time_domain_configuration.item_type(), errors);
                    break;
                }
                default: {
                    time::shared::validate_properties(domain, properties,
                        space_domain_configuration, errors);
                    break;
                }
            }

            break;
        }
    }
}

}  // namespace constant_size


std::unique_ptr<PropertySetConfiguration> validate_property_set_configuration(
    hdf5::Group const& property_set,
    Errors& errors)
{
    std::unique_ptr<PropertySetConfiguration> configuration;

    assert_attributes_exist(property_set, {
            "lue_size_of_item_collection_type"
        }, errors);


    if(property_set.contains_attribute(
            "lue_size_of_item_collection_type")) {
        try {
            auto const size_of_item_collection_type =
                parse_size_of_item_collection_type(
                    property_set.attribute<std::string>(
                        "lue_size_of_item_collection_type"));

            configuration = std::make_unique<PropertySetConfiguration>(
                size_of_item_collection_type);
        }
        catch(std::exception const& exception) {
            errors.emplace_back(property_set.id(), exception.what());
        }
    }

    return configuration;
}


std::unique_ptr<TimeDomainConfiguration> validate_time_domain_configuration(
    hdf5::Group const& time_domain,
    Errors& errors)
{
    std::unique_ptr<TimeDomainConfiguration> configuration;

    assert_attributes_exist(time_domain, {
            "domain_type",
            "domain_item_type",
        }, errors);


    if(time_domain.contains_attribute("domain_type") &&
        time_domain.contains_attribute("domain_item_type")) {

        try {
            auto const time_domain_type = parse_time_domain_type(
                time_domain.attribute<std::string>("domain_type"));
            auto const time_domain_item_type = parse_time_domain_item_type(
                time_domain.attribute<std::string>("domain_item_type"));

            configuration = std::make_unique<TimeDomainConfiguration>(
                time_domain_type, time_domain_item_type);
        }
        catch(std::exception const& exception) {
            errors.emplace_back(time_domain.id(), exception.what());
        }
    }

    return configuration;
}


std::unique_ptr<SpaceDomainConfiguration> validate_space_domain_configuration(
    hdf5::Group const& space_domain,
    Errors& errors)
{
    std::unique_ptr<SpaceDomainConfiguration> configuration;

    assert_attributes_exist(space_domain, {
            "domain_type",
            "domain_item_type",
            "mobility"
        }, errors);


    if(space_domain.contains_attribute("domain_type") &&
        space_domain.contains_attribute("mobility") &&
        space_domain.contains_attribute("domain_item_type")) {

        try {
            auto const space_domain_type = parse_space_domain_type(
                space_domain.attribute<std::string>("domain_type"));
            auto const space_domain_item_type =
                parse_space_domain_item_type(
                    space_domain.attribute<std::string>(
                        "domain_item_type"));
            auto const mobility = parse_mobility(
                space_domain.attribute<std::string>("mobility"));

            configuration = std::make_unique<SpaceDomainConfiguration>(
                space_domain_type, space_domain_item_type, mobility);
        }
        catch(std::exception const& exception) {
            errors.emplace_back(space_domain.id(), exception.what());
        }
    }

    return configuration;
}


void validate_property_set(
    hdf5::Group const& property_set,
    Errors& errors)
{
    size_t const nr_errors = errors.size();

    auto property_set_configuration = validate_property_set_configuration(
        property_set, errors);


    assert_groups_exist(property_set, {
            "lue_domain",
            "lue_properties"
        }, errors);


    if(property_set.contains_group("lue_domain")) {

        auto const domain = hdf5::open_group(property_set, "lue_domain");

        assert_groups_exist(domain, {
                "lue_time",
                "lue_space"
            }, errors);


        std::unique_ptr<TimeDomainConfiguration> time_domain_configuration;

        if(domain.contains_group("lue_time")) {
            auto const time_domain = hdf5::open_group(domain, "lue_time");

            time_domain_configuration =
                validate_time_domain_configuration(time_domain, errors);
        }


        std::unique_ptr<SpaceDomainConfiguration> space_domain_configuration;

        if(domain.contains_group("lue_space")) {
            auto const space_domain = hdf5::open_group(domain, "lue_space");

            space_domain_configuration =
                validate_space_domain_configuration(space_domain, errors);
        }


        if(property_set.contains_group("lue_properties")) {

            auto const properties = hdf5::open_group(property_set,
                "lue_properties");

            // Collection of properties. Each group must be a valid property.
            // No other groups are allowed.
            auto const property_names = hdf5::group_names(properties);

            for(auto const& property_name: property_names) {
                auto const property = hdf5::open_group(properties,
                    property_name);

                assert_soft_links_exist(property, {
                        "lue_domain"
                    }, errors);

                // Make sure the domain pointed to by each property
                // equals the property set's domain.
                assert_link_points_to(property, "lue_domain", domain, errors);
            }

            if(errors.size() == nr_errors && property_set_configuration &&
                    time_domain_configuration && space_domain_configuration) {

                switch(property_set_configuration->
                        size_of_item_collection_type()) {
                    case SizeOfItemCollectionType::constant_size: {
                        constant_size::validate_properties(domain,
                            properties, *time_domain_configuration,
                            *space_domain_configuration, errors);
                        break;
                    }
                }
            }
        }
    }
}


void validate_property_sets(
    hdf5::Group const& group,
    Errors& errors)
{
    // Collection of property sets. Each group must be a valid property set.
    // No other groups are allowed.
    auto group_names = hdf5::group_names(group);

    for(auto const& name: group_names) {
        validate_property_set(hdf5::open_group(group, name), errors);
    }
}


void validate_phenomenon(
    hdf5::Group const& group,
    Errors& errors)
{
    assert_groups_exist(group, {
            "lue_property_sets"
        }, errors);

    if(group.contains_group("lue_property_sets")) {
        validate_property_sets(hdf5::open_group(group, "lue_property_sets"),
            errors);
    }
}


void validate_phenomena(
    hdf5::Group const& group,
    Errors& errors)
{
    // Collection of phenomena. Each group must be a valid phenomenon.
    // No other groups are allowed.
    auto group_names = hdf5::group_names(group);

    for(auto const& name: group_names) {
        validate_phenomenon(hdf5::open_group(group, name), errors);
    }
}


void validate_universe(
    hdf5::Group const& group,
    Errors& errors)
{
    assert_groups_exist(group, {
            "lue_phenomena"
        }, errors);

    if(group.contains_group("lue_phenomena")) {
        validate_phenomena(hdf5::open_group(group, "lue_phenomena"), errors);
    }
}


void validate_universes(
    hdf5::Group const& group,
    Errors& errors)
{
    // Collection of universes. Each group must be a valid universe.
    // No other groups are allowed.
    auto group_names = hdf5::group_names(group);

    for(auto const& name: group_names) {
        validate_universe(hdf5::open_group(group, name), errors);
    }
}

}  // Anonymous namespace


void validate_dataset(
    hdf5::File const& file,
    Errors& errors)
{
    assert_groups_exist(file, {
            "lue_universes",
            "lue_phenomena"
        }, errors);

    if(file.contains_group("lue_universes")) {
        validate_universes(hdf5::open_group(file, "lue_universes"), errors);
    }

    if(file.contains_group("lue_phenomena")) {
        validate_phenomena(hdf5::open_group(file, "lue_phenomena"), errors);
    }
}

}  // namespace lue
