#include "lue/validate/validate.hpp"
#include "lue/validate.hpp"
#include <fmt/format.h>


namespace {

std::string const usage = fmt::format(R"(
Validate LUE datasets

usage:
    {0} [--issues] [--fail-on-warning] <dataset>...
    {0} (-h | --help) | --version

options:
    -h --help   Show this screen
    --version   Show version
    --issues    Only print issues
    --fail-on-warning   Treat warnings as errors
)", "lue_validate");

} // Anonymous namespace


namespace lue {
namespace utility {

Validate::Validate(
    std::vector<std::string> const& arguments)

    : Application(usage, arguments)

{
    // Turn off error stack traversal. The default functions prints
    // lots of messages we usually don't care about.
    H5Eset_auto(H5E_DEFAULT, nullptr, nullptr);
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
        try {
            data_model::assert_is_valid(dataset_name, fail_on_warning);

            if(!only_print_issues) {
                print_info_message("File " + dataset_name +
                    " contains a valid LUE dataset");
            }
        }
        catch(std::runtime_error const& exception) {
            print_info_message("File " + dataset_name +
                " does not contain a valid LUE dataset:");
            errors_found = true;
            print_info_message(exception.what());
        }
    }

    return errors_found ? EXIT_FAILURE : EXIT_SUCCESS;
}

}  // namespace utility
}  // namespace lue
