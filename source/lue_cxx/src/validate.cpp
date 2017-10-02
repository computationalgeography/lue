#include "lue/validate.hpp"
#include "lue/constant_size/time/omnipresent/different_shape/property.hpp"
#include "lue/constant_size/time/omnipresent/property_set.hpp"
#include "lue/constant_size/time/omnipresent/same_shape/property.hpp"
#include "lue/dataset.hpp"
#include "lue/tag.hpp"
#include <boost/format.hpp>


namespace lue {
namespace {

void missing_group(
    hdf5::Identifier const& id,
    std::string const& name,
    Issues& issues)
{
    issues.add_error(id, "group '" + name + "' does not exist");
}


void superflouos_object(
    hdf5::Identifier const& id,
    std::string const& name,
    Issues& issues)
{
    issues.add_error(id, "object '" + name + "' is superfluous");
}


template<
    typename Collection>
Collection difference(
    Collection collection1,
    Collection collection2)
{
    std::sort(collection1.begin(), collection1.end());
    std::sort(collection2.begin(), collection2.end());

    Collection result;

    std::set_difference(
        collection1.begin(), collection1.end(),
        collection2.begin(), collection2.end(),
        std::inserter(result, result.begin()));

    return result;
}


void assert_no_superfluous_objects_exist(
    hdf5::Group const& group,
    std::vector<std::string> const& names,
    Issues& issues)
{
    auto superflouos_objects_names = difference(group.object_names(), names);

    // These objects are not expected.
    for(auto const& name: superflouos_objects_names) {
        superflouos_object(group.id(), name, issues);
    }
}


bool assert_groups_exist(
    hdf5::Group const& group,
    std::vector<std::string> const& names,
    Issues& issues)
{
    auto missing_groups_names = difference(names, group.group_names());

    // These required groups are not found.
    for(auto const& name: missing_groups_names) {
        missing_group(group.id(), name, issues);
    }

    return missing_groups_names.empty();
}

}  // Anonymous namespace


// We are in the lue namespace now. This makes the code below easier to write.
// We need less explicit namespace qualifications.
namespace constant_size {
namespace time {
namespace omnipresent {
// namespace same_shape {
// 
// }  // namespace same_shape


void validate(
    PropertySet const& property_set,
    Issues& issues)
{
    // TODO
    // - test Number of item ids equals number of values in each property

    auto const nr_ids = property_set.ids().nr_items();

    auto const& properties = property_set.properties();

    for(auto const& name: properties.names()) {
        auto const& property = properties[name];
        auto const& configuration = property.configuration();
        size_t nr_values;

        switch(configuration.shape_per_item_type()) {
            case ShapePerItemType::same: {
                auto file_datatype = same_shape::Property::file_datatype(
                    property.id());
                auto const property2 = same_shape::Property(
                    property, memory_datatype(file_datatype));
                nr_values =  property2.values().nr_items();

    //             // to_dot(same_shape::Property(property,
    //             //     memory_datatype(file_datatype)), issues);

                break;
            }
            case ShapePerItemType::different: {
                auto file_datatype = different_shape::Property::file_datatype(
                    property.id());
                auto const property2 = different_shape::Property(
                    property, memory_datatype(file_datatype));
                nr_values =  property2.values().nr_items();

    //             // to_dot(different_shape::Property(property,
    //             //     memory_datatype(file_datatype)), issues);

                break;
            }
        }

        if(nr_values != nr_ids) {
            issues.add_error(
                property.id(),
                boost::str(boost::format(
                    "property %1%: nr property values (%2%) != "
                    "nr property-set ids (%3%)")
                    % name
                    % nr_values
                    % nr_ids
                )
            );
        }
    }
}

}  // namespace omnipresent
}  // namespace time
}  // namespace constant_size


void validate(
    PropertySet const& property_set,
    Issues& issues)
{
    auto configuration = property_set.configuration();

    switch(configuration.size_of_item_collection_type()) {
        // TODO Switch on time domain, once possible.

        case(SizeOfItemCollectionType::constant_size): {
            validate(constant_size::time::omnipresent::PropertySet(
                property_set.id()), issues);
            break;
        }
    }
}


void validate(
    Phenomenon const& phenomenon,
    Issues& issues)
{
    auto const& property_sets = phenomenon.property_sets();

    for(auto const& name: property_sets.names()) {
        validate(property_sets[name], issues);
    }
}


void validate(
    Universe const& universe,
    Issues& issues)
{
    auto const& phenomena = universe.phenomena();

    for(auto const& name: phenomena.names()) {
        validate(phenomena[name], issues);
    }
}


void validate(
    Dataset const& dataset,
    Issues& issues)
{
    // TODO
    // - test version attribute (error)
    // - test history attribute (warning)


    auto const& universes = dataset.universes();

    for(auto const& name: universes.names()) {
        validate(universes[name], issues);
    }

    auto const& phenomena = dataset.phenomena();

    for(auto const& name: phenomena.names()) {
        validate(phenomena[name], issues);
    }
}


void validate(
    hdf5::File const& file,
    Issues& issues)
{
    // TODO
    //     history attribute on all levels, maybe with only information
    //     about the contained information?
    // std::vector<std::string> expected_attribute_names = {
    //     history_tag,
    //     version_tag,
    // };
    // assert_attributes_exist(file, expected_attribute_names, issues);


    std::vector<std::string> expected_group_names = {
        phenomena_tag,
        universes_tag,
    };
    auto const groups_exist = assert_groups_exist(
        file, expected_group_names, issues);


    if(groups_exist) {
        try {
            validate(Dataset(file.pathname()), issues);
        }
        catch(std::exception const& exception) {
            issues.add_error(file.id(), exception.what());
        }
    }


    std::vector<std::string> expected_object_names = expected_group_names;
    assert_no_superfluous_objects_exist(file, expected_object_names, issues);





    // if(file.contains_group("lue_universes")) {
    //     validate_universes(hdf5::open_group(file, "lue_universes"), issues);
    // }


    // if(file.contains_group("lue_phenomena")) {
    //     validate_phenomena(hdf5::open_group(file, "lue_phenomena"), issues);
    // }




    // std::vector<std::string> expected_object_names = expected_group_names;
    // assert_no_superfluous_objects_exist(file, expected_object_names, issues);


    // auto const& universes = dataset.universes();

    // for(auto const& name: universes.names()) {
    //     validate(universes[name], issues);
    // }

    // auto const& phenomena = dataset.phenomena();

    // for(auto const& name: phenomena.names()) {
    //     validate(phenomena[name], issues);
    // }
}


void validate(
    std::string const& name,
    Issues& issues)
{
    validate(hdf5::File(name), issues);
}

}  // namespace lue
