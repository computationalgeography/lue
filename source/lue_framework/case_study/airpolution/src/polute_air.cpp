#include "polute_air.hpp"
// #include <hpx/config.hpp>
#include <hpx/hpx.hpp>
// #include <hpx/lcos/gather.hpp>
// #include "lue/data_model.hpp"
// #include <hpx/runtime.hpp>
#include <fmt/format.h>
#include <iostream>
#include <cassert>


namespace {

char const* gather_basename = "/polute_air/gather/";


hpx::future<double> max_airpolution(
    std::uint64_t const /* nr_time_steps */,
    std::uint64_t const /* nr_rows */,
    std::uint64_t const /* nr_cols */,
    std::uint64_t const /* nr_rows_grain */,
    std::uint64_t const /* nr_cols_grain */)
{
    // TODO Perform some real HPX work

    using namespace std::chrono_literals;

    auto const nr_localities = hpx::get_num_localities().get();

    std::this_thread::sleep_for(10s / nr_localities);

    return hpx::make_ready_future<double>(5.0);
}

}  // Anonymous namespace


HPX_REGISTER_GATHER(double, max_airpolution_gatherer);


namespace lue {

void polute_air(
    std::uint64_t const nr_time_steps,
    std::uint64_t const nr_rows,
    std::uint64_t const nr_cols,
    std::uint64_t const nr_rows_grain,
    std::uint64_t const nr_cols_grain)
{
    assert(nr_rows > 0);
    assert(nr_cols > 0);
    assert(nr_rows_grain < nr_rows);
    assert(nr_cols_grain < nr_cols);

    auto const locality_id = hpx::get_locality_id();

    // std::cout << "locality: " << locality_id << std::endl;

    // TODO Do something useful on the current locality
    hpx::future<double> local_result = max_airpolution(
        nr_time_steps, nr_rows, nr_cols, nr_rows_grain, nr_cols_grain);

    // TODO Gather results from all localities

    if(locality_id == 0) {
        // We are the gather site
        auto const nr_localities = hpx::get_num_localities().get();
        auto const nr_worker_threads = hpx::get_num_worker_threads();

        // std::cout << "nr_worker_threads: " << nr_worker_threads << std::endl;

        std::vector<double> all_results =
            hpx::lcos::gather_here(
                gather_basename,
                std::move(local_result),
                nr_localities).get();

        assert(!all_results.empty());

        auto const overall_result =
            *std::max_element(all_results.begin(), all_results.end());

        std::cout << fmt::format(
            "Received {} results, of which the maximum is {}\n",
            nr_localities, overall_result);
    }
    else {
        // We are not the gather site

        // Transmit value to gather site (locality_id == 0)
        hpx::lcos::gather_there(
            gather_basename, std::move(local_result)).wait();
    }
}

}  // namespace lue
