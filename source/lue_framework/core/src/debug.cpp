#include "lue/framework/core/debug.hpp"
#include <hpx/include/compute.hpp>
#include <hpx/include/lcos.hpp>
#include <hpx/parallel/algorithms/transform.hpp>
#include <fmt/format.h>
#include <algorithm>


namespace lue {
namespace {

/*!
    @brief      Return a future to a string representing an
                hpx::naming::id_type instance
*/
hpx::future<std::string> id_to_str(
    hpx::naming::id_type const id)
{
    return hpx::get_locality_name(id);
}


/*!
    @brief      Return a future to a string containing the strings
                pointed to by the futures in @a collection
*/
template<
    template<typename> typename Collection>
hpx::future<std::string> join(
    Collection<hpx::future<std::string>>&& collection,
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

    hpx::parallel::transform(
        hpx::parallel::execution::par,
        locality_ids.begin(), locality_ids.end(), locality_names.begin(),
        [](auto const locality_id)
        {
            return id_to_str(locality_id);
        });

    return locality_names;
}

}  // Anonymous namespace


hpx::future<std::string> system_description()
{
    // Locality where this function is called on
    hpx::naming::id_type const this_locality_id = hpx::find_here();
    hpx::future<std::string> this_locality_name = id_to_str(this_locality_id);

    // The root locality is the locality where the main AGAS service is hosted
    hpx::naming::id_type const root_locality_id = hpx::find_root_locality();
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
        std::string const& all_locality_names
        )
    {
        return fmt::format(
                    "this_locality_nr     : {}\n"
                    "this_locality_name   : {}\n"
                    "root_locality_name   : {}\n"
                    "----------------------\n"
                    "initial_nr_localities: {}\n"
                    "current_nr_localities: {}\n"
                    "all_locality_names   : {}\n"
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
                numa_domains.size(),
                nr_os_threads,
                thread_name,
                this_worker_thread_nr
            );
    };

    hpx::future<std::string> description =
        hpx::dataflow(
            hpx::util::unwrapping(format_message),
            this_locality_name, root_locality_name,
            current_nr_localities, all_locality_names);

    return description;
}

}  // namespace lue
