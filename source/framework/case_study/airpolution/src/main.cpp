#include "pollute_air.hpp"
#include "lue/framework/core/configuration_entry.hpp"
#include <hpx/hpx_init.hpp>


int hpx_main(int const /* argc */, char* /* argv */[])
{
    // using Shape = std::vector<std::uint64_t>;
    using Shape = lue::Shape<lue::Count, 2>;

    // Runtime configuration
    // std::uint64_t const max_tree_depth =
    //     lue::required_configuration_entry<std::uint64_t>("max_tree_depth");

    // Simulation configuration
    lue::Count const nr_time_steps = lue::required_configuration_entry<lue::Count>("nr_time_steps");

    Shape const array_shape = lue::required_configuration_entry<Shape>("array_shape");
    Shape const partition_shape = lue::required_configuration_entry<Shape>("partition_shape");

    lue::pollute_air(nr_time_steps, array_shape, partition_shape);

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
