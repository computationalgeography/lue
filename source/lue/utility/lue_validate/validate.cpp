#include "lue/utility/lue_validate/validate.h"
#include "lue/utility/lue_validate/validator.h"
#include "lue/cxx_api/hdf5/file.h"
#include <map>


namespace lue {

Validate::Validate(
    int argc,
    char* argv[],
    std::string const& usage)

    : Application(argc, argv, usage)

{
}


void Validate::print_errors(
    Errors const& errors) const
{
    // Aggregate all errors by id. Print errors per id.

    std::vector<hdf5::Identifier> ids;
    std::map<hdf5::Identifier, std::vector<std::string>> messages;

    for(auto const& error: errors) {
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


void Validate::run_implementation()
{
    auto const input_dataset_names = argument<std::vector<std::string>>(
        "<input>");

    for(auto const& input_dataset_name: input_dataset_names) {
        auto const file = hdf5::open_file(input_dataset_name);
        Errors errors;
        validate_dataset(file, errors);

        if(errors.empty()) {
            print_info_message("File " + input_dataset_name +
                " contains a valid LUE dataset");
        }
        else {
            print_info_message("File " + input_dataset_name +
                " does not contain a valid LUE dataset:");
            print_errors(errors);
        }
    }
}

}  // namespace lue
