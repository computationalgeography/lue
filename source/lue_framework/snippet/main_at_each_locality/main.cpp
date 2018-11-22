#include <hpx/hpx_init.hpp>
#include <hpx/include/iostreams.hpp>
#include <fmt/format.h>


namespace {

}  // Anonymous namespace


int hpx_main(
    int const /* argc */,
    char* /* argv */ [])
{
    auto const locality = hpx::get_locality_id();
    auto const nr_worker_threads = hpx::get_num_worker_threads();
    auto const localities = hpx::find_all_localities();

    hpx::cout << fmt::format(
            "Running on locality {} using {} threads and "
            "seeing {} localities\n",
            locality, nr_worker_threads, localities.size());

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
