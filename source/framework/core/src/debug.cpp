#include "lue/framework/core/debug.hpp"
#include "lue/framework/configure.hpp"
#include <hpx/include/compute.hpp>
#ifdef LUE_HPX_WITH_MPI
#include <hpx/mpi_base/mpi_environment.hpp>
#endif
#include <hpx/include/lcos.hpp>
#include <hpx/parallel/algorithms/transform.hpp>
#include "hpx/runtime_distributed/find_localities.hpp"
#include "hpx/runtime_distributed/get_locality_name.hpp"
#include <algorithm>
#include <sstream>


namespace lue {
namespace {

/*!
    @brief      Return a future to a string representing an
                hpx::id_type instance
*/
hpx::future<std::string> id_to_str(
    hpx::id_type const id)
{
    return hpx::get_locality_name(id);
}


/*!
    @brief      Return a future to a string containing the strings
                pointed to by the futures in @a collection
*/
template<
    template<typename, typename...> typename Collection,
    typename... Args>
hpx::future<std::string> join(
    Collection<hpx::future<std::string>, Args...>&& collection,
    std::string const& separator)
{
    // Attach a continuation that grabs all strings pointed to by the
    // futures in the collection passed in, and joins them into a
    // new string. A future to this new string is returned.
    return hpx::when_all_n(
        std::begin(collection), std::size(collection)).then(
            [separator](auto&& future)
            {
                auto name_futures = future.get();
                std::vector<std::string> names(name_futures.size());

                std::transform(
                    name_futures.begin(), name_futures.end(), names.begin(),
                    [](auto& name_future)
                    {
                        return name_future.get();
                    });

                return fmt::format("{}", fmt::join(
                    std::begin(names), std::end(names), separator));
            });
}


/*!
    @brief      Return a collection of futures to strings containing
                the names of all localities
*/
std::vector<hpx::future<std::string>> all_locality_names()
{
    auto const locality_ids = hpx::find_all_localities();
    std::vector<hpx::future<std::string>> locality_names(locality_ids.size());

    // TODO(KDJ) For some reason this triggers a compiler error on Github CI build on Windows...
    //      Commented for now.
    // hpx::transform(
    //     hpx::execution::par,
    //     locality_ids.begin(), locality_ids.end(), locality_names.begin(),
    //     [](auto const locality_id) -> hpx::future<std::string>
    //     {
    //         return id_to_str(locality_id);
    //     });

    return locality_names;
}


#ifdef LUE_HPX_WITH_MPI
std::string locality_rank_mapping()
{
    std::ostringstream stream;
    stream << hpx::get_locality_id();

    return fmt::format(
            "locality: {} -- rank(HPX): {}",  // -- rank(env): {}",
            stream.str(),
            hpx::util::mpi_environment::rank()
            // , std::getenv("OMPI_COMM_WORLD_RANK")
        );
}
#endif

}  // Anonymous namespace
}  // namespace lue


#ifdef LUE_HPX_WITH_MPI
HPX_PLAIN_ACTION(lue::locality_rank_mapping, LocalityRankMapping)
#endif


namespace lue {


hpx::future<std::string> system_description()
{
    // Locality where this function is called on
    hpx::id_type const this_locality_id = hpx::find_here();
    hpx::future<std::string> this_locality_name = id_to_str(this_locality_id);

    // The root locality is the locality where the main AGAS service is hosted
    hpx::id_type const root_locality_id = hpx::find_root_locality();
    hpx::future<std::string> root_locality_name = id_to_str(root_locality_id);

    auto const this_locality_nr = hpx::get_locality_id();
    auto const initial_nr_localities = hpx::get_initial_num_localities();

    // Number of localities currently registered for the running application
    auto current_nr_localities = hpx::get_num_localities();

    auto numa_domains = hpx::compute::host::numa_domains();

    // Number of worker OS-threads in use by this locality
    auto const nr_os_threads = hpx::get_os_thread_count();

    auto const thread_name = hpx::get_thread_name();

    // Returns the OS-thread number of the worker that is running this
    // HPX-thread
    auto const this_worker_thread_nr = hpx::get_worker_thread_num();

    // auto all_locality_names = lue::all_locality_names();
    auto all_locality_names = join(lue::all_locality_names(), ", ");


#ifdef LUE_HPX_WITH_MPI
        auto const locality_ids = hpx::find_all_localities();
        std::vector<hpx::future<std::string>> locality_rank_mappings(locality_ids.size());
        LocalityRankMapping action{};

        hpx::transform(
            hpx::execution::par,
            locality_ids.begin(), locality_ids.end(), locality_rank_mappings.begin(),
            [action](auto const locality_id)
            {
                return hpx::async(action, locality_id);
            });

        auto all_locality_rank_mappings = join(std::move(locality_rank_mappings), ", ");
#endif


    auto format_message = [
            this_locality_nr,
            initial_nr_localities,
            numa_domains{move(numa_domains)},
            nr_os_threads,
            thread_name,
            this_worker_thread_nr
        ](
        std::string const& this_locality_name,
        std::string const& root_locality_name,
        auto const current_nr_localities,
        std::string const& all_locality_names,
        [[maybe_unused]] std::string const& all_locality_rank_mappings)
    {
        return fmt::format(
                    "this_locality_nr     : {}\n"
                    "this_locality_name   : {}\n"
                    "root_locality_name   : {}\n"
                    "----------------------\n"
                    "initial_nr_localities: {}\n"
                    "current_nr_localities: {}\n"
                    "all_locality_names   : {}\n"
                    "all_locality_rank_mappings: {}\n"
                    "nr_numa_domains      : {}\n"
                    "----------------------\n"
                    "nr_os_threads        : {}\n"
                    "thread_name          : {}\n"
                    "this_worker_thread_nr: {}\n"
                ,
                this_locality_nr,
                this_locality_name,
                root_locality_name,
                initial_nr_localities,
                current_nr_localities,
                all_locality_names,
                all_locality_rank_mappings,
                numa_domains.size(),
                nr_os_threads,
                thread_name,
                this_worker_thread_nr
            );
    };

    hpx::future<std::string> description =
        hpx::dataflow(
            hpx::unwrapping(format_message),
            this_locality_name, root_locality_name,
            current_nr_localities, all_locality_names,
#ifdef LUE_HPX_WITH_MPI
            all_locality_rank_mappings
#else
            "-"
#endif
            );

    return description;
}


void write_debug_message(
    std::string const& message)
{
    std::cerr << "LUE DEBUG: " << message << '\n';
}

}  // namespace lue
