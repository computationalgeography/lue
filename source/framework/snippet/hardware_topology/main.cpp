#include <hpx/hpx_init.hpp>
#include <hpx/iostream.hpp>
// #include <hpx/runtime/resource/partitioner.hpp>
// TODO The above header doesn't provide detail::partitioner class definition
#include <hpx/resource_partitioner/detail/partitioner.hpp>
#include <fmt/format.h>


namespace {

}  // Anonymous namespace


int hpx_main(
    int const /* argc */,
    char* /* argv */ [])
{
    auto const locality = hpx::get_locality_id();
    auto const& partitioner = hpx::resource::get_partitioner();
    auto const& numa_domains = partitioner.numa_domains();

    hpx::cout << fmt::format(
        "Locality {} with {} numa domains:\n",
        locality, std::size(numa_domains));

    for(auto const& domain: numa_domains) {
        hpx::cout << fmt::format("    domain: {}\n", domain.id());

        for(auto const& core: domain.cores()) {
            hpx::cout << fmt::format("        core: {}\n", core.id());

            for(auto const& pu: core.pus()) {
                hpx::cout << fmt::format("            pu: {}\n", pu.id());
            }
        }
    }

    return hpx::finalize();
}


int main(
    int const argc,
    char* argv[])
{
    std::vector<std::string> cfg = {
        "hpx.run_hpx_main!=1"  // Run hpx_main on all localities
    };

    return hpx::init(argc, argv, cfg);
}
