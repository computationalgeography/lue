#include <hpx/hpx_init.hpp>
#include "lue/framework/core/configuration_entry.hpp"
#include "polute_air.hpp"


int hpx_main(
    int const argc,
    char* argv[])
{
    std::uint64_t const nr_time_steps =
        lue::required_configuration_entry<std::uint64_t>("nr_time_steps");
    std::uint64_t const nr_rows =
        lue::required_configuration_entry<std::uint64_t>("nr_rows");
    std::uint64_t const nr_cols =
        lue::required_configuration_entry<std::uint64_t>("nr_cols");
    std::uint64_t const nr_rows_partition =
        lue::required_configuration_entry<std::uint64_t>("nr_rows_partition");
    std::uint64_t const nr_cols_partition =
        lue::required_configuration_entry<std::uint64_t>("nr_cols_partition");

    lue::polute_air(
        nr_time_steps, nr_rows, nr_cols, nr_rows_partition, nr_cols_partition);

    return hpx::finalize();
}


int main(
    int const argc,
    char* argv[])
{
    // // Turn off error stack traversal. The default functions prints
    // // lots of messages we usually don't care about.
    // H5Eset_auto(H5E_DEFAULT, nullptr, nullptr);

    std::vector<std::string> cfg = {
        // // Run hpx_main on all localities
        // "hpx.run_hpx_main!=1",
        // "application.lue_polute_air.mah!=1"
    };

    return hpx::init(argc, argv, cfg);
}
