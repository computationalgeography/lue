#include <hpx/hpx_init.hpp>
#include "burn_wilderness.hpp"
#include "lue/framework/core/configuration_entry.hpp"
#include "lue/framework/benchmark/hpx_main.hpp"  // create_task


int hpx_main(
    int const /* argc */,
    char* /* argv */ [])
{
    lue::benchmark::Task task{lue::benchmark::detail::create_task()};

    std::uint64_t max_tree_depth =
        lue::optional_configuration_entry<std::uint64_t>(
            "benchmark.max_tree_depth", 0);

    if(max_tree_depth == 0) {
        max_tree_depth = task.nr_time_steps();
    }

    lue::benchmark::burn_wilderness(task, max_tree_depth);

    return hpx::finalize();
}


int main(
    int const argc,
    char* argv[])
{
    return hpx::init(argc, argv);  // , cfg);
}
