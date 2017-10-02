#include "lue/validate/validate.hpp"
#include "lue/constant_size/time/omnipresent/different_shape/property.hpp"
#include "lue/constant_size/time/omnipresent/property_set.hpp"
#include "lue/constant_size/time/omnipresent/same_shape/property.hpp"
#include "lue/dataset.hpp"
#include "lue/tag.hpp"
#include <boost/format.hpp>


namespace {

std::string const usage = boost::str(boost::format(R"(
Validate LUE datasets

usage:
    %1% [--issues] [--fail-on-warning] <dataset>...
    %1% (-h | --help) | --version

options:
    -h --help   Show this screen
    --version   Show version
    --issues    Only print issues
    --fail-on-warning   Treat warnings as errors
)") % "lue_validate");

} // Anonymous namespace


namespace lue {
namespace utility {
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
}  // namespace utility


using namespace utility;


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


namespace utility {

Validate::Validate(
    std::vector<std::string> const& arguments)

    : Application(usage, arguments)

{
    // Turn off error stack traversal. The default functions prints
    // lots of messages we usually don't care about.
    H5Eset_auto1(NULL, NULL);
}


void Validate::print_issues(
    Issues const& issues) const
{
    // Aggregate all issues by id. Print issues per id.
    // The idea is to print important messages first.

    // Collection of ids, ordered by errors.
    std::vector<hdf5::Identifier> ids;

    // Mapping of id to messages.
    std::map<hdf5::Identifier, std::vector<std::string>> messages;

    // Handle all errors. These will be printed first.
    for(auto const& error: issues.errors()) {
        if(messages.find(error.id()) == messages.end()) {
            ids.push_back(error.id());
        }

        messages[error.id()].push_back("error: " + error.message());
    }

    // Handle all warnings. Warnings will be added to the list of messages
    // created above. For objects without errors, the warnings will be
    // stored last.
    for(auto const& warning: issues.warnings()) {
        if(messages.find(warning.id()) == messages.end()) {
            ids.push_back(warning.id());
        }

        messages[warning.id()].push_back("warning: " + warning.message());
    }


    // Print the issues. First the errors (and possibly warnings) are printed,
    // and after that the warnings for objects without errors.
    for(auto const& id: ids) {
        print_info_message(id.pathname() + ":");

        for(auto const& message: messages[id]) {
            print_info_message("- " + message);
        }

        // print_info_message("");
    }
}


int Validate::run_implementation()
{
    // Verify that each dataset passed in is valid.
    auto const dataset_names =
        argument<std::vector<std::string>>("<dataset>");
    bool errors_found = false;
    auto const only_print_issues = argument<bool>("--issues");
    auto const fail_on_warning = argument<bool>("--fail-on-warning");

    for(auto const& dataset_name: dataset_names) {
        Issues issues;

        auto const file = hdf5::File(dataset_name);

        validate(hdf5::File(dataset_name), issues);

        if(issues.errors_found() || (
                fail_on_warning && issues.warnings_found())) {
            print_info_message("File " + dataset_name +
                " does not contain a valid LUE dataset:");
            errors_found = true;
        }
        else if(!only_print_issues) {
            print_info_message("File " + dataset_name +
                " contains a valid LUE dataset");
        }

        // Print issues found, if any
        print_issues(issues);
    }

    return errors_found ? EXIT_FAILURE : EXIT_SUCCESS;
}

}  // namespace utility
}  // namespace lue
