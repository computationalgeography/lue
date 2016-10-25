#include "lue/utility/lue_validate/validate.h"
#include "lue/cxx_api/space_domain_configuration.h"
#include "lue/cxx_api/value_configuration.h"
#include "lue/cxx_api/time_domain_configuration.h"
#include <boost/format.hpp>
#include <iostream>
#include <map>
// Only use the LUE/HDF5 API, not the LUE C++ API itself!


namespace lue {

Validate::Validate(
    int argc,
    char* argv[],
    std::string const& usage)

    : Application(argc, argv, usage)

{
}


void Validate::print_errors() const
{
    // Aggregate all errors by id. Print errors per id.

    std::vector<hdf5::Identifier> ids;
    std::map<hdf5::Identifier, std::vector<std::string>> messages;

    for(auto const& error: _errors) {
        if(messages.find(error.id()) == messages.end()) {
            ids.push_back(error.id());
        }

        messages[error.id()].push_back(error.message());
    }

    for(auto const& id: ids) {
        print_info_message(id.pathname() + ":");

        for(auto const& message: messages[id]) {
            print_info_message("- " + message);
        }

        print_info_message("");
    }
}


void Validate::not_implemented(
    std::string const& message) const
{
    print_info_message("Not implemented yet: " + message);
}


void Validate::missing_group(
    hdf5::Identifier const& id,
    std::string const& name) const
{
    _errors.emplace_back(id, "group '" + name + "' does not exist");
}


void Validate::missing_dataset(
    hdf5::Identifier const& id,
    std::string const& name) const
{
    _errors.emplace_back(id, "dataset '" + name + "' does not exist");
}


void Validate::missing_attribute(
    hdf5::Identifier const& id,
    std::string const& name) const
{
    _errors.emplace_back(id, "attribute '" + name + "' does not exist");
}


void Validate::missing_soft_link(
    hdf5::Identifier const& id,
    std::string const& name) const
{
    _errors.emplace_back(id, "soft link '" + name + "' does not exist");
}


void Validate::link_does_not_point_to_group(
    hdf5::Group const& group,
    std::string const& name,
    hdf5::Group const& target) const
{
    _errors.emplace_back(group.id(), "soft link '" + name +
        "' does not point to " + target.id().pathname());
}


void Validate::invalid_mobility(
    hdf5::Group const& group,
    SpaceDomainType const domain_type,
    Mobility const mobility) const
{
    _errors.emplace_back(group.id(), boost::str(boost::format(
        "space domain type '%1%' cannot be combined with mobility '%2%'")
        % space_domain_type_to_string(domain_type)
        % mobility_to_string(mobility)));
}


void Validate::invalid_domain_item_type(
    hdf5::Group const& group,
    SpaceDomainType const domain_type,
    SpaceDomainItemType const domain_item_type) const
{
    _errors.emplace_back(group.id(), boost::str(boost::format(
        "space domain type '%1%' cannot be combined with "
        "domain item type '%2%'")
        % space_domain_type_to_string(domain_type)
        % space_domain_item_type_to_string(domain_item_type)));
}


void Validate::invalid_domain_item_type(
    hdf5::Group const& group,
    TimeDomainType const domain_type,
    TimeDomainItemType const domain_item_type) const
{
    _errors.emplace_back(group.id(), boost::str(boost::format(
        "time domain type '%1%' cannot be combined with "
        "domain item type '%2%'")
        % time_domain_type_to_string(domain_type)
        % time_domain_item_type_to_string(domain_item_type)));
}


void Validate::assert_groups_exist(
    hdf5::Group const& group,
    std::vector<std::string> const& names) const
{
    for(auto const& name: names) {
        if(!group.contains_group(name)) {
            missing_group(group.id(), name);
        }
    }
}


void Validate::assert_datasets_exist(
    hdf5::Group const& group,
    std::vector<std::string> const& names) const
{
    for(auto const& name: names) {
        if(!group.contains_dataset(name)) {
            missing_dataset(group.id(), name);
        }
    }
}


void Validate::assert_shape_equals(
    hdf5::Dataset const& dataset,
    hdf5::Dataspace const& dataspace,
    Shape const& shape) const
{
    auto const extents = dataspace.dimension_extents();

    for(size_t d = 0; d < extents.size(); ++d) {
        // If a dimension in shape's extent is 0, that means that the
        // extent can have any size.
        if(shape[d] != 0 && extents[d] != shape[d]) {
            _errors.emplace_back(dataset.id(), boost::str(boost::format(
                "extent of dimension '%1%' is '%2%', but must be '%3%'")
                % d
                % extents[d]
                % shape[d]));
        }
    }
}


void Validate::assert_shape_equals(
    hdf5::Dataset const& dataset,
    Shape const& shape) const
{
    assert_shape_equals(dataset, dataset.dataspace(), shape);
}


void Validate::assert_rank_equals(
    hdf5::Dataset const& dataset,
    hdf5::Dataspace const& dataspace,
    size_t const rank) const
{
    if(static_cast<Shape::size_type>(dataspace.nr_dimensions()) != rank) {
        _errors.emplace_back(dataset.id(), boost::str(boost::format(
            "rank is '%1%', but must be '%2%'")
            % dataspace.nr_dimensions()
            % rank));
    }
}


void Validate::assert_rank_equals(
    hdf5::Dataset const& dataset,
    size_t const rank) const
{
    assert_rank_equals(dataset, dataset.dataspace(), rank);
}


void Validate::assert_dataspace_equals(
    hdf5::Dataset const& dataset,
    Shape const& shape) const
{
    auto const dataspace = dataset.dataspace();

    assert_rank_equals(dataset, dataspace, shape.size());

    if(static_cast<Shape::size_type>(dataspace.nr_dimensions()) ==
            shape.size()) {
        assert_shape_equals(dataset, dataspace, shape);
    }
}


void Validate::assert_datatype_equals(
    hdf5::Dataset const& dataset,
    hid_t const datatype) const
{
    auto const dataset_datatype = dataset.type_id();

    if(!hdf5::datatypes_are_equal(dataset_datatype, datatype)) {
        _errors.emplace_back(dataset.id(), boost::str(boost::format(
            "datatype is '%1%', but must be '%2%'")
            % hdf5::standard_datatype_as_string(dataset_datatype)
            % hdf5::standard_datatype_as_string(datatype)));
    }
}


void Validate::assert_attributes_exist(
    hdf5::Group const& group,
    std::vector<std::string> const& names) const
{
    for(auto const& name: names) {
        if(!group.contains_attribute(name)) {
            missing_attribute(group.id(), name);
        }
    }
}


void Validate::assert_soft_links_exist(
    hdf5::Group const& group,
    std::vector<std::string> const& names) const
{
    for(auto const& name: names) {
        if(!group.contains_soft_link(name)) {
            missing_soft_link(group.id(), name);
        }
    }
}


void Validate::assert_link_points_to(
    hdf5::Group const& group,
    std::string const& name,
    hdf5::Group const& target) const
{
    if(hdf5::open_group(group, name) != target) {
        link_does_not_point_to_group(group, name, target);
    }
}


// void Validate::validate_domains(
//     hdf5::Group const& group) const
// {
//     // Here we can assume that the time and space domains themselves are
//     // valid. We must check whether the *combination* of the time and
//     // space domains is valid.
// 
//     auto const time_domain = hdf5::open_group(group, "time");
//     auto const space_domain = hdf5::open_group(group, "space");
// 
//     auto const time_domain_type = parse_time_domain_type(
//         time_domain.attribute<std::string>("domain_type"));
//     // auto const time_domain_item_type = parse_time_domain_item_type(
//     //     time_domain.attribute<std::string>("domain_item_type"));
// 
//     // auto const space_domain_type = parse_space_domain_type(
//     //     space_domain.attribute<std::string>("domain_type"));
//     auto const mobility = parse_mobility(
//         space_domain.attribute<std::string>("mobility"));
//     // auto const space_domain_item_type = parse_space_domain_item_type(
//     //     space_domain.attribute<std::string>("domain_item_type"));
// 
// 
//     // time_domain_type:
//     // - omnipresent
//     // - shared_constant_collection
// 
//     // time_domain_item_type:
//     // - none
//     // - period
// 
//     // space_domain_type:
//     // - omnipresent
//     // - located
// 
//     // mobility:
//     // - stationary
//     // - mobile
// 
//     // space_domain_item_type:
//     // - none
//     // - box
// 
// 
//     // Illegal combinations:
// 
//     // omnipresent time domain + mobile space items
//     if(time_domain_type == TimeDomainType::omnipresent &&
//             mobility == Mobility::mobile) {
//         _errors.emplace_back(group.id(),
//             "mobility requires a located time domain");
//     }
// }
// 
// 
// void Validate::validate_space_domain(
//     hdf5::Group const& group,
//     SpaceDomainType const domain_type,
//     Mobility const mobility,
//     SpaceDomainItemType const domain_item_type) const
// {
//     // space_domain_type:
//     // - omnipresent
//     // - located
// 
//     // mobility:
//     // - stationary
//     // - mobile
// 
//     // space_domain_item_type:
//     // - none
//     // - box
// 
// 
//     // Illegal combinations:
//     // - omnipresent + mobile
//     // - omnipresent + not-none
//     // - located + none
// 
// 
//     switch(domain_type) {
//         case SpaceDomainType::omnipresent: {
//             switch(mobility) {
//                 case Mobility::stationary: {
//                     switch(domain_item_type) {
//                         case SpaceDomainItemType::none: {
//                             // Nothing to validate.
//                             break;
//                         }
//                         default: {
//                             invalid_domain_item_type(group, domain_type,
//                                 domain_item_type);
//                             break;
//                         }
//                     }
// 
//                     break;
//                 }
//                 case Mobility::mobile: {
//                     invalid_mobility(group, domain_type, mobility);
// 
//                     switch(domain_item_type) {
//                         case SpaceDomainItemType::none: {
//                             // TODO
//                             not_implemented("omnipresent/mobile/none");
//                             break;
//                         }
//                         case SpaceDomainItemType::box: {
//                             // TODO
//                             not_implemented("omnipresent/mobile/box");
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
//         case SpaceDomainType::located: {
//             switch(mobility) {
//                 case Mobility::stationary: {
//                     switch(domain_item_type) {
//                         case SpaceDomainItemType::none: {
//                             invalid_domain_item_type(group, domain_type,
//                                 domain_item_type);
//                             break;
//                         }
//                         case SpaceDomainItemType::box: {
//                             // TODO
// 
// std::cout << "located/stationary/box\n";
// 
// 
// 
//                             break;
//                         }
//                     }
// 
//                     break;
//                 }
//                 case Mobility::mobile: {
//                     switch(domain_item_type) {
//                         case SpaceDomainItemType::none: {
//                             invalid_domain_item_type(group, domain_type,
//                                 domain_item_type);
//                             break;
//                         }
//                         case SpaceDomainItemType::box: {
//                             // TODO
// 
//                             not_implemented("located/mobile/box");
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
// 
// 
// void Validate::validate_space_domain(
//     hdf5::Group const& group) const
// {
//     assert_attributes_exist(group, {
//         "domain_type",
//         "domain_item_type",
//         "mobility"
//     });
// 
//     try {
//         if(group.contains_attribute("domain_type")) {
//             auto const domain_type_string = group.attribute<std::string>(
//                 "domain_type");
//             auto const domain_type = parse_space_domain_type(
//                 domain_type_string);
// 
//             if(group.contains_attribute("mobility")) {
//                 auto const mobility_string = group.attribute<std::string>(
//                     "mobility");
//                 auto const mobility = parse_mobility(mobility_string);
// 
//                 if(group.contains_attribute("domain_type")) {
//                     auto const domain_item_type_string =
//                         group.attribute<std::string>("domain_item_type");
//                     auto const domain_item_type =
//                         parse_space_domain_item_type(domain_item_type_string);
// 
//                     validate_space_domain(group, domain_type, mobility,
//                         domain_item_type);
//                 }
//             }
//         }
//     }
//     catch(std::exception const& exception) {
//         _errors.emplace_back(group.id(), exception.what());
//     }
// }
// 
// 
// void Validate::validate_sc_periods(
//     hdf5::Group const& group) const
// {
//     assert_attributes_exist(group, {
//         "resolution"
//     });
// 
//     // TODO
// 
//     // try {
//     //     if(group.contains_attribute("resolution") {
//     //         auto const resolution_string =
//     //             group.attribute<std::string>("resolution");
//     //         auto const resolution =
//     //             parse_time_domain_item_type(
//     //                 resolution_string);
// 
//     //         validate_time_domain(group, domain_type, resolution);
// 
//     //     }
//     // }
//     // catch(std::exception const& exception) {
//     //     _errors.emplace_back(group.id(), exception.what());
//     // }
// }
// 
// 
// void Validate::validate_time_domain(
//     hdf5::Group const& group,
//     TimeDomainType const domain_type,
//     TimeDomainItemType const domain_item_type) const
// {
//     // time_domain_type:
//     // - omnipresent
//     // - shared_constant_collection
// 
//     // time_domain_item_type:
//     // - none
//     // - period
// 
// 
//     // Illegal combinations:
//     // - omnipresent + not-none
//     // - shared_constant_collection + none
// 
// 
//     switch(domain_type) {
//         case TimeDomainType::omnipresent: {
//             switch(domain_item_type) {
//                 case TimeDomainItemType::none: {
//                     // Nothing to validate.
//                     break;
//                 }
//                 case TimeDomainItemType::period: {
//                     invalid_domain_item_type(group, domain_type,
//                         domain_item_type);
//                     break;
//                 }
//             }
// 
//             break;
//         }
//         case TimeDomainType::shared_constant_collection: {
//             switch(domain_item_type) {
//                 case TimeDomainItemType::none: {
//                     invalid_domain_item_type(group, domain_type,
//                         domain_item_type);
//                     break;
//                 }
//                 case TimeDomainItemType::period: {
//                     validate_sc_periods(group);
//                     break;
//                 }
//             }
// 
//             break;
//         }
//     }
// }
// 
// 
// void Validate::validate_time_domain(
//     hdf5::Group const& group) const
// {
//     assert_attributes_exist(group, {
//         "domain_type",
//         "domain_item_type"
//     });
// 
// 
//     try {
//         if(group.contains_attribute("domain_type")) {
//             auto const domain_type_string = group.attribute<std::string>(
//                 "domain_type");
//             auto const domain_type = parse_time_domain_type(
//                 domain_type_string);
// 
//             if(group.contains_attribute("domain_type")) {
//                 auto const domain_item_type_string =
//                     group.attribute<std::string>("domain_item_type");
//                 auto const domain_item_type = parse_time_domain_item_type(
//                     domain_item_type_string);
// 
//                 validate_time_domain(group, domain_type, domain_item_type);
//             }
//         }
//     }
//     catch(std::exception const& exception) {
//         _errors.emplace_back(group.id(), exception.what());
//     }
// }
// 
// 
// void Validate::validate_domain(
//     hdf5::Group const& group) const
// {
//     // Assume there are no properties. Only test the domain itself.
// 
//     // Each domain must contain information about the time and space
//     // domains.
// 
//     size_t const nr_errors = _errors.size();
// 
//     assert_groups_exist(group, {
//         "time",
//         "space"
//     });
// 
//     if(group.contains_group("time")) {
//         validate_time_domain(hdf5::open_group(group, "time"));
//     }
// 
//     if(group.contains_group("space")) {
//         validate_space_domain(hdf5::open_group(group, "space"));
//     }
// 
//     bool const domains_are_valid = nr_errors == _errors.size();
// 
// 
//     // Iff time and space domains are valid, validate whether the combination
//     // is valid.
//     if(domains_are_valid) {
//         validate_domains(group);
//     }
// }
// 
// 
// void Validate::validate_property(
//     hdf5::Group const& group) const
// {
//     assert_soft_links_exist(group, {
//         "domain"
//     });
// 
//     // validate_properties verifies that the property domain equals
//     // the property set domain. It also calls validate_domain on the
//     // property set domain. So we don't have to do that here.
// 
//     // Test the domain and the property are a valid combination.
//     // Assume that the time and space domain themselves are validated
//     // elsewhere.
//     not_implemented("domain + property combination");
// }


void Validate::validate_cs_value(
    hdf5::Group const& domain,
    hdf5::Group const& value) const
{
    // assert_groups_exist(property, {
    //     "value"
    // });


    // if(property.contains_group("value")) {
    //     auto const value = hdf5::open_group(property, "value");

        assert_datasets_exist(value, {
            "value"
        });


        if(value.contains_dataset("value")) {
            auto const dataset = hdf5::open_dataset(value, "value");

            // Shape of dataset must be at least 2D. First dimension is
            // for items, subsequent dimensions are for values.

            auto const dataspace = dataset.dataspace();

            if(dataspace.nr_dimensions() <= 1) {
                _errors.emplace_back(dataset.id(), boost::str(boost::format(
                    "number of dimensions is '%1%', "
                    "but must be at least '%2%'")
                    % dataspace.nr_dimensions()
                    % 2));
            }
            else {
                // Number of items in dataset must equal number of
                // items in domain/ids.

                if(domain.contains_dataset("ids")) {
                    auto const ids_dataset = open_dataset(domain, "ids");
                    auto const ids_dataspace = ids_dataset.dataspace();
                    auto const ids_extents =
                        ids_dataspace.dimension_extents();

                    auto const extents = dataspace.dimension_extents();

                    if(extents[0] != ids_extents[0]) {

                        _errors.emplace_back(dataset.id(), boost::str(
                            boost::format(
                                "extent of dimension '%1%' is '%2%', "
                                "but must be '%3%'")
                            % 0
                            % extents[0]
                            % ids_extents[0]));
                    }
                }
            }
        }
    // }
}


void Validate::validate_vs_value(
    hdf5::Group const& domain,
    hdf5::Group const& value) const
{
    // time: omnipresent
    // space:
    // - located
    // - each item has different shape

    // assert_groups_exist(property, {
    //     "value"
    // });


    // if(property.contains_group("value")) {
    //     auto const value = hdf5::open_group(property, "value");

        assert_groups_exist(value, {
            "value"
        });


        if(value.contains_group("value")) {
    //         auto const dataset = hdf5::open_dataset(value, "value");

    //         // Shape of dataset must be at least 2D. First dimension is
    //         // for items, subsequent dimensions are for values.

    //         auto const dataspace = dataset.dataspace();

    //         if(dataspace.nr_dimensions() <= 1) {
    //             _errors.emplace_back(dataset.id(), boost::str(boost::format(
    //                 "number of dimensions is '%1%', "
    //                 "but must be at least '%2%'")
    //                 % dataspace.nr_dimensions()
    //                 % 2));
    //         }
    //         else {
    //             // Number of items in dataset must equal number of
    //             // items in domain/ids.

    //             if(domain.contains_dataset("ids")) {
    //                 auto const ids_dataset = open_dataset(domain, "ids");
    //                 auto const ids_dataspace = ids_dataset.dataspace();
    //                 auto const ids_extents =
    //                     ids_dataspace.dimension_extents();

    //                 auto const extents = dataspace.dimension_extents();

    //                 if(extents[0] != ids_extents[0]) {

    //                     _errors.emplace_back(dataset.id(), boost::str(
    //                         boost::format(
    //                             "extent of dimension '%1%' is '%2%', "
    //                             "but must be '%3%'")
    //                         % 0
    //                         % extents[0]
    //                         % ids_extents[0]));
    //                 }
    //             }
    //         }
        }
    // }
}


void Validate::o_validate_property(
    hdf5::Group const& group) const
{
    // Property
    // - nD value per item
    // - possibly discretized through space, if so
    //     - xxx

    auto const domain = hdf5::open_group(group, "domain");

    assert_groups_exist(group, {
        "value"
    });


    if(group.contains_group("value")) {
        auto const value = hdf5::open_group(group, "value");

        assert_attributes_exist(value, {
            "lue_shape_per_item_type"
        });


        if(value.contains_attribute("lue_shape_per_item_type")) {
            try {
                auto const shape_per_item_type = parse_shape_per_item_type(
                    value.attribute<std::string>("lue_shape_per_item_type"));

                switch(shape_per_item_type) {
                    case ShapePerItemType::same_shape: {
                        validate_cs_value(domain, value);
                        break;
                    }
                    case ShapePerItemType::different_shape: {
                        validate_vs_value(domain, value);
                        break;
                    }
                }


                // TODO
                if(value.contains_soft_link("lue_space_discretization")) {
                    // Value is discretized.
                    auto const discretization = hdf5::open_group(value,
                        "lue_space_discretization");
                }

            }
            catch(std::exception const& exception) {
                _errors.emplace_back(value.id(), exception.what());
            }
        }
    }
}


void Validate::o_none_validate_properties(
    hdf5::Group const& domain,
    hdf5::Group const& group) const
{
    // omnipresent/omnipresent/stationary/none
    // Validate layout of the items.


    // Time domain
    // Empty


    // Space domain
    // Empty


    // Items
    // - Id collection in domain/ids dataset
    // - dataset
    //     - shape: (unlimited)
    //     - type: uint64_t
    assert_datasets_exist(domain, {
        "ids"
    });


    if(domain.contains_dataset("ids")) {
        auto const dataset = open_dataset(domain, "ids");
        assert_dataspace_equals(dataset, Shape{0});
        assert_datatype_equals(dataset, H5T_STD_U64LE);
    }


    // Property
    // - nD value per item
    // - not discretized
    auto const group_names = hdf5::group_names(group);

    for(auto const& name: group_names) {
        auto const property = hdf5::open_group(group, name);

        o_validate_property(property);
    }
}


void Validate::o_box_validate_properties(
    hdf5::Group const& domain,
    hdf5::Group const& group) const
{
    // omnipresent/located/stationary/box
    // Validate layout of the items.


    // Time domain
    // Empty


    // Space domain
    // Per item a box
    // Dataset 'coordinates'
    // Shape: 2D:
    //     shape[0]: extent represents items
    //     shape[1]: 2 * space dimensionality numbers representing 2 corners
    //         of the box
    // TODO space/rank attribute
    auto const space_domain = hdf5::open_group(domain, "space");

    assert_datasets_exist(space_domain, {
        "coordinates"
    });

    if(space_domain.contains_dataset("coordinates")) {
        const int rank = 2;

        auto const dataset = hdf5::open_dataset(space_domain,
            "coordinates");
        auto const dataspace = dataset.dataspace();

        assert_rank_equals(dataset, 2);
        assert_shape_equals(dataset, Shape{0, 2 * rank});
        assert_datatype_equals(dataset, H5T_IEEE_F64LE);
    }


    // Items
    // - Id collection in domain/ids dataset
    // - dataset
    //     - shape: (unlimited)
    //     - type: uint64_t
    assert_datasets_exist(domain, {
        "ids"
    });


    if(domain.contains_dataset("ids")) {
        auto const dataset = open_dataset(domain, "ids");
        assert_dataspace_equals(dataset, Shape{0});
        assert_datatype_equals(dataset, H5T_STD_U64LE);
    }


    auto const group_names = hdf5::group_names(group);

    for(auto const& name: group_names) {
        auto const property = hdf5::open_group(group, name);

        o_validate_property(property);
    }
}


void Validate::sc_p_s_box_validate_properties(
    hdf5::Group const& domain,
    hdf5::Group const& group) const
{
    // shared_constant/period/located/stationary/box
    // Validate layout of the items.

    try {
        // Time domain
        // - Resolution attribute
        // - 2D dataset
        //     - first dimension is time domain item dimension
        //     - extent of second dimension depends on resolution:
        //         - month: 3: year, month, nr_months
        auto const time_domain = hdf5::open_group(domain, "time");

        assert_datasets_exist(time_domain, {
            "coordinates"
        });


        if(time_domain.contains_dataset("coordinates")) {
            auto const dataset = hdf5::open_dataset(time_domain,
                "coordinates");
            auto const dataspace = dataset.dataspace();

            assert_rank_equals(dataset, 2);

            assert_attributes_exist(time_domain, {
                "lue_resolution"
            });

            // if(time_domain.contains_attribute("resolution")) {
            //     auto const time_resolution = parse_time_resolution(
            //         time_domain.attribute<std::string>("resolution"));

            //     if(time_resolution = TimeResolution::Month) {
            //             auto const extents = dataspace.dimension_extents();


            //     }
            //     else {
            //         not_implemented("time resolution " +
            //             time_resolution_as_string(time_resolution));
            //     }
            // }
        }


        // Space domain


        // Items


        // Property




    }
    catch(std::exception const& exception) {
        _errors.emplace_back(domain.id(), exception.what());
    }
}


void Validate::o_validate_properties(
    hdf5::Group const& domain,
    hdf5::Group const& group,
    SpaceDomainType const space_domain_type,
    Mobility const mobility,
    SpaceDomainItemType const space_domain_item_type) const
{
    auto const space_domain = hdf5::open_group(domain, "space");

    switch(space_domain_type) {
        case SpaceDomainType::omnipresent: {
            switch(mobility) {
                case Mobility::stationary: {
                    switch(space_domain_item_type) {
                        case SpaceDomainItemType::none: {
                            o_none_validate_properties(domain, group);
                            break;
                        }
                        default: {
                            invalid_domain_item_type(space_domain,
                                space_domain_type, space_domain_item_type);
                            break;
                        }
                    }

                    break;
                }
                case Mobility::mobile: {
                    invalid_mobility(space_domain, space_domain_type,
                        mobility);
                    break;
                }
            }

            break;
        }
        case SpaceDomainType::located: {
            switch(mobility) {
                case Mobility::stationary: {
                    switch(space_domain_item_type) {
                        case SpaceDomainItemType::none: {
                            invalid_domain_item_type(space_domain,
                                space_domain_type, space_domain_item_type);
                            break;
                        }
                        case SpaceDomainItemType::box: {
                            o_box_validate_properties(domain, group);
                            break;
                        }
                    }

                    break;
                }
                case Mobility::mobile: {
                    invalid_mobility(space_domain, space_domain_type,
                        mobility);
                    break;
                }
            }

            break;
        }
    }
}


void Validate::sc_p_validate_properties(
    hdf5::Group const& domain,
    hdf5::Group const& group,
    SpaceDomainType const space_domain_type,
    Mobility const mobility,
    SpaceDomainItemType const space_domain_item_type) const
{
    auto const space_domain = hdf5::open_group(domain, "space");

    switch(space_domain_type) {
        case SpaceDomainType::omnipresent: {
            switch(mobility) {
                case Mobility::stationary: {
                    switch(space_domain_item_type) {
                        case SpaceDomainItemType::none: {
                            // TODO sc_p_none_validate_properties(domain,
                            //     group);
                            not_implemented("shared_constant/period/"
                                "omnipresent/stationary/none");
                            break;
                        }
                        default: {
                            invalid_domain_item_type(space_domain,
                                space_domain_type, space_domain_item_type);
                            break;
                        }
                    }

                    break;
                }
                case Mobility::mobile: {
                    invalid_mobility(space_domain, space_domain_type,
                        mobility);
                    break;
                }
            }

            break;
        }
        case SpaceDomainType::located: {
            switch(mobility) {
                case Mobility::stationary: {
                    switch(space_domain_item_type) {
                        case SpaceDomainItemType::none: {
                            invalid_domain_item_type(space_domain,
                                space_domain_type, space_domain_item_type);
                            break;
                        }
                        case SpaceDomainItemType::box: {
                            sc_p_s_box_validate_properties(domain, group);
                            break;
                        }
                    }

                    break;
                }
                case Mobility::mobile: {
                    switch(space_domain_item_type) {
                        case SpaceDomainItemType::none: {
                            invalid_domain_item_type(space_domain,
                                space_domain_type, space_domain_item_type);
                            break;
                        }
                        case SpaceDomainItemType::box: {
                            // TODO sc_p_m_box_validate_properties(domain,
                            //     group);
                            not_implemented("shared_constant/period/"
                                "located/mobile/box");
                            break;
                        }
                    }

                    break;
                }
            }

            break;
        }
    }
}


void Validate::validate_properties(
    hdf5::Group const& domain,
    hdf5::Group const& group,
    TimeDomainType const time_domain_type,
    TimeDomainItemType const time_domain_item_type,
    SpaceDomainType const space_domain_type,
    Mobility const mobility,
    SpaceDomainItemType const space_domain_item_type) const
{
    auto const time_domain = hdf5::open_group(domain, "time");

    switch(time_domain_type) {
        case TimeDomainType::omnipresent: {
            switch(time_domain_item_type) {
                case TimeDomainItemType::none: {
                    o_validate_properties(domain, group,
                        space_domain_type, mobility, space_domain_item_type);
                    break;
                }
                default: {
                    invalid_domain_item_type(time_domain, time_domain_type,
                        time_domain_item_type);
                    break;
                }
            }

            break;
        }
        case TimeDomainType::shared: {
            switch(time_domain_item_type) {
                case TimeDomainItemType::none: {
                    invalid_domain_item_type(time_domain, time_domain_type,
                        time_domain_item_type);
                    break;
                }
                case TimeDomainItemType::period: {
                    sc_p_validate_properties(domain, group,
                        space_domain_type, mobility, space_domain_item_type);
                    break;
                }
            }

            break;
        }
    }
}


void Validate::validate_properties(
    hdf5::Group const& domain,
    hdf5::Group const& group) const
{
    // Collection of properties. Each group must be a valid property.
    // No other groups are allowed.
    auto const group_names = hdf5::group_names(group);

    for(auto const& name: group_names) {
        auto const property = hdf5::open_group(group, name);

        // Make sure the domain pointed to by each property
        // equals the property set's domain.
        assert_link_points_to(property, "domain", domain);
    }


    assert_groups_exist(domain, {
        "time",
        "space"
    });

    if(domain.contains_group("time") && domain.contains_group("space")) {
        auto const time_domain = hdf5::open_group(domain, "time");
        auto const space_domain = hdf5::open_group(domain, "space");

        assert_attributes_exist(time_domain, {
            "domain_type",
            "domain_item_type",
        });


        assert_attributes_exist(space_domain, {
            "domain_type",
            "domain_item_type",
            "mobility"
        });


        if(time_domain.contains_attribute("domain_type") &&
            time_domain.contains_attribute("domain_item_type") &&
            space_domain.contains_attribute("domain_type") &&
            space_domain.contains_attribute("mobility") &&
            space_domain.contains_attribute("domain_item_type")) {

            try {
                auto const time_domain_type = parse_time_domain_type(
                    time_domain.attribute<std::string>("domain_type"));
                auto const time_domain_item_type = parse_time_domain_item_type(
                    time_domain.attribute<std::string>("domain_item_type"));

                auto const space_domain_type = parse_space_domain_type(
                    space_domain.attribute<std::string>("domain_type"));
                auto const mobility = parse_mobility(
                    space_domain.attribute<std::string>("mobility"));
                auto const space_domain_item_type =
                    parse_space_domain_item_type(
                        space_domain.attribute<std::string>(
                            "domain_item_type"));

                validate_properties(domain, group,
                    time_domain_type, time_domain_item_type,
                    space_domain_type, mobility, space_domain_item_type);
            }
            catch(std::exception const& exception) {
                _errors.emplace_back(time_domain.id(), exception.what());
            }
        }
    }









    // size_t const nr_errors = _errors.size();

    // validate_domain(domain);

    // bool const domain_is_valid = nr_errors == _errors.size();

    // if(domain_is_valid) {
    //     auto group_names = hdf5::group_names(group);

    //     for(auto const& name: group_names) {
    //         auto const property = hdf5::open_group(group, name);

    //         // Make sure the domain pointed to by each property
    //         // equals the property set's domain.
    //         assert_link_points_to(property, "domain", domain);

    //         // The domain is validated allready, so validate_property
    //         // doesn't have to call validate_domain.
    //         validate_property(property);
    //     }
    // }

    // not_implemented("collection of items");
}


void Validate::validate_property_set(
    hdf5::Group const& group) const
{
    assert_attributes_exist(group, {
        "lue_size_of_item_collection_type"
    });

    assert_groups_exist(group, {
        "domain",
        "properties"
    });

    // TODO Forward/handle size of item collection type.
    if(group.contains_group("domain") &&
            group.contains_group("properties")) {
        validate_properties(
            hdf5::open_group(group, "domain"),
            hdf5::open_group(group, "properties"));
    }
}


void Validate::validate_property_sets(
    hdf5::Group const& group) const
{
    // Collection of property sets. Each group must be a valid property set.
    // No other groups are allowed.
    auto group_names = hdf5::group_names(group);

    for(auto const& name: group_names) {
        validate_property_set(hdf5::open_group(group, name));
    }
}


void Validate::validate_phenomenon(
    hdf5::Group const& group) const
{
    assert_groups_exist(group, {
        "property_sets"
    });

    if(group.contains_group("property_sets")) {
        validate_property_sets(hdf5::open_group(group, "property_sets"));
    }
}


void Validate::validate_phenomena(
    hdf5::Group const& group) const
{
    // Collection of phenomena. Each group must be a valid phenomenon.
    // No other groups are allowed.
    auto group_names = hdf5::group_names(group);

    for(auto const& name: group_names) {
        validate_phenomenon(hdf5::open_group(group, name));
    }
}


void Validate::validate_universe(
    hdf5::Group const& group) const
{
    assert_groups_exist(group, {
        "phenomena"
    });

    if(group.contains_group("phenomena")) {
        validate_phenomena(hdf5::open_group(group, "phenomena"));
    }
}


void Validate::validate_universes(
    hdf5::Group const& group) const
{
    // Collection of universes. Each group must be a valid universe.
    // No other groups are allowed.
    auto group_names = hdf5::group_names(group);

    for(auto const& name: group_names) {
        validate_universe(hdf5::open_group(group, name));
    }
}


void Validate::validate_dataset(
    std::string const& filename) const
{
    auto file = hdf5::open_file(filename);

    assert_groups_exist(file, {
        "universes",
        "phenomena"
    });

    if(file.contains_group("universes")) {
        validate_universes(hdf5::open_group(file, "universes"));
    }

    if(file.contains_group("phenomena")) {
        validate_phenomena(hdf5::open_group(file, "phenomena"));
    }


    if(_errors.empty()) {
        print_info_message("File contains a valid LUE dataset");
    }
    else {
        print_errors();
        print_info_message("File does not contain a valid LUE dataset");
    }
}


void Validate::run_implementation()
{
    std::string const input_dataset_name = argument<std::string>("<input>");

    print_verbose_message("validate dataset");

    validate_dataset(input_dataset_name);
}

}  // namespace lue
