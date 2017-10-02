#include "lue/validate/validate.hpp"
#include "lue/validate.hpp"
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

        // auto const file = hdf5::File(dataset_name);

        // validate(hdf5::File(dataset_name), issues);

        validate(dataset_name, issues);

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
