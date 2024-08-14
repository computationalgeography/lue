#include "change_land_use.hpp"
#include "lue/framework/benchmark/hpx_main.hpp"  // create_task
#include "lue/framework/core/configuration_entry.hpp"
#include <hpx/hpx_init.hpp>


int hpx_main(int const /* argc */, char* /* argv */[])
{
    lue::benchmark::Task task{lue::benchmark::detail::create_task()};

    std::uint64_t max_tree_depth =
        lue::optional_configuration_entry<std::uint64_t>("benchmark.max_tree_depth", 0);

    if (max_tree_depth == 0)
    {
        max_tree_depth = task.nr_time_steps();
    }

    lue::benchmark::change_land_use(task, max_tree_depth);

    return hpx::finalize();
}


int main(int const argc, char* argv[])
{
    // // Turn off error stack traversal. The default functions prints
    // // lots of messages we usually don't care about.
    // H5Eset_auto(H5E_DEFAULT, nullptr, nullptr);

    std::vector<std::string> cfg = {
        // // Run hpx_main on all localities
        // "hpx.run_hpx_main!=1",
        // "application.lue_pollute_air.mah!=1"
    };

    return hpx::init(argc, argv, cfg);
}
