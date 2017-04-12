#include "lue_translate/command/export.h"
#include "lue_translate/format.h"
#include <boost/filesystem.hpp>
#include <exception>


namespace lue {
namespace utility {
namespace {

std::string const usage = R"(
Translate data from the LUE dataset format

usage:
    export <input> <output>
    export (-h | --help)

arguments:
    <input>     Input dataset
    <output>    Output dataset

options:
    -h --help   Show this screen
)";

}  // Anonymous namespace


std::string const Export::name = "export";


Command::CommandPtr Export::command(
    std::vector<std::string> const& arguments)
{
    return std::make_unique<Export>(arguments);
}


Export::Export(
    std::vector<std::string> const& arguments)

    : Command(usage, arguments)

{
}


void Export::run_implementation()
{
    namespace bfs = boost::filesystem;

    std::string const input_dataset_name = argument<std::string>("<input>");
    std::string const output_dataset_name = argument<std::string>("<output>");

    if(auto lue_dataset = try_open_lue_dataset_for_read(input_dataset_name)) {

        // Input is a dataset that can be read by LUE.
        // We need to convert from the LUE format to some other format.

        if(bfs::path(output_dataset_name).extension() == ".dot") {
            // Create a Graphviz DOT graph of the dataset.
            translate_lue_dataset_to_dot(*lue_dataset, output_dataset_name);
        }
        else {
            throw std::runtime_error(
                "translation to " + output_dataset_name +
                " is not supported");
        }
    }
}

}  // namespace utility
}  // namespace lue
