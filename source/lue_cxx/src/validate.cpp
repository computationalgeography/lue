#include "lue/validate.hpp"
#include "lue/constant_size/validate.hpp"
#include "lue/dataset.hpp"
#include "lue/tag.hpp"
#include "lue/hdf5/validate.hpp"
#include <boost/format.hpp>
#include <sstream>


namespace lue {
namespace {

std::string error_message(
    hdf5::Issues const& issues)
{
    std::stringstream stream;

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
        stream << id.pathname() << ":\n";

        for(auto const& message: messages[id]) {
            stream << "- " << message << "\n";
        }
    }

    return stream.str();
}

}  // Anonymous namespace


void validate(
    PropertySet const& property_set,
    hdf5::Issues& issues)
{
    auto configuration = property_set.configuration();

    switch(configuration.size_of_item_collection_type()) {
        // TODO Switch on time domain, once possible.

        case(SizeOfItemCollectionType::constant_size): {
            constant_size::validate(constant_size::PropertySet(
                property_set.id()), issues);
            break;
        }
    }
}


void validate(
    Phenomenon const& phenomenon,
    hdf5::Issues& issues)
{
    auto const& property_sets = phenomenon.property_sets();

    for(auto const& name: property_sets.names()) {
        validate(property_sets[name], issues);
    }
}


void validate(
    Universe const& universe,
    hdf5::Issues& issues)
{
    auto const& phenomena = universe.phenomena();

    for(auto const& name: phenomena.names()) {
        validate(phenomena[name], issues);
    }
}


void validate(
    Dataset const& dataset,
    hdf5::Issues& issues)
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
    hdf5::Issues& issues)
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
    auto const groups_exist = hdf5::assert_groups_exist(
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
    hdf5::assert_no_superfluous_objects_exist(
        file, expected_object_names, issues);





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
    hdf5::Issues& issues)
{
    validate(hdf5::File(name), issues);
}


void assert_is_valid(
    hdf5::File const& file,
    bool const fail_on_warning)
{
    hdf5::Issues issues;

    validate(file, issues);

    if(issues.errors_found() || (fail_on_warning && issues.warnings_found())) {
        throw std::runtime_error(error_message(issues));
    }
}


void assert_is_valid(
    std::string const& dataset_name,
    bool const fail_on_warning)
{
    assert_is_valid(hdf5::File(dataset_name), fail_on_warning);
}

}  // namespace lue
