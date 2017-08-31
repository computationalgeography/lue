#include "lue/translate/command/export.hpp"
#include "lue/translate/format.hpp"
#include <boost/filesystem.hpp>
#include <exception>


namespace lue {
namespace utility {
namespace {

std::string const usage = R"(
Translate data from the LUE dataset format

usage:
    export [-m <name>] <input> <output>
    export (-h | --help)

arguments:
    <input>     Input dataset
    <output>    Output dataset

options:
    -h --help   Show this screen
    -m <name> --meta=<name>  File containing metadata to use during import
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

    bool const metadata_passed = argument_passed("--meta");

    auto const metadata = metadata_passed
        ? Metadata(argument<std::string>("--meta"))
        : Metadata();


    if(auto lue_dataset = try_open_lue_dataset_for_read(input_dataset_name)) {

        // Input is a dataset that can be read by LUE.
        // We need to convert from the LUE format to some other format.

        if(bfs::path(output_dataset_name).extension() == ".dot") {
            // Create a Graphviz DOT graph of the dataset.
            translate_lue_dataset_to_dot(*lue_dataset, output_dataset_name,
                metadata);
        }
        else if(bfs::path(output_dataset_name).extension() == ".vtk") {
            // Create a VTK file of the dataset.
            translate_lue_dataset_to_vtk(*lue_dataset, output_dataset_name,
                metadata);
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
