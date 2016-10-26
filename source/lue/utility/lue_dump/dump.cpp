#include "lue/utility/lue_dump/dump.h"
#include "lue/utility/lue_dump/txt_format.h"
#include "lue/cxx_api/dataset.h"
#include <iostream>


namespace lue {

Dump::Dump(
    int argc,
    char* argv[],
    std::string const& usage)

    : Application(argc, argv, usage)

{
}


void Dump::run_implementation()
{
    std::string const input_dataset_name = argument<std::string>("<input>");
    std::string const format = argument<std::string>("--format");

    if(format == "txt") {
        auto const dataset = open_dataset(input_dataset_name);
        describe_dataset(dataset, std::cout);
    }
    else {
        throw std::runtime_error("unsupported format: " + format);
    }
}

}  // namespace lue
