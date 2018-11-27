#include <hpx/hpx_init.hpp>
#include "polute_air.hpp"
#include <docopt.h>
#include <fmt/format.h>
#include <boost/filesystem.hpp>
#include <iostream>


namespace {

std::string const usage = R"({0}

usage:
    {0} <dataset>
)";

}  // Anonymous namespace


int hpx_main(
    int const argc,
    char* argv[])
{
    auto const basename = boost::filesystem::path(argv[0]).filename().string();
    auto const arguments =
        docopt::docopt(fmt::format(usage, basename), { argv + 1, argv + argc });

    auto const dataset_pathname = arguments.at("<dataset>").asString();


    lue::polute_air(dataset_pathname);

    return hpx::finalize();
}


int main(
    int const argc,
    char* argv[])
{
    std::vector<std::string> cfg = {
        // "hpx.numa_sensitive=2"  // no-cross NUMA stealing
        "hpx.run_hpx_main!=1"  // Run hpx_main on all localities
    };

    return hpx::init(argc, argv, cfg);
}
