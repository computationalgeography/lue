#include "polute_air.hpp"
#include "lue/framework/core/debug.hpp"
#include "lue/framework/core/domain_decomposition.hpp"
// #include <hpx/config.hpp>
#include <hpx/hpx.hpp>
// #include <hpx/lcos/gather.hpp>
// #include "lue/data_model.hpp"
// #include <hpx/runtime.hpp>
#include <hpx/include/iostreams.hpp>
#include <fmt/format.h>
#include <iostream>
#include <cassert>


namespace {

// char const* gather_basename()
// {
//     // static char const* basename = "/polute_air/gather/";
//     // static auto const nr_localities = hpx::get_num_localities().get();
//     // static std::string const basename =
//     //     fmt::format("/{}/polute_air/gather/", nr_localities);
// 
//     static std::string const basename = "/polute_air/gather/";
// 
//     return basename.c_str();
// }


// hpx::future<double> max_airpolution(
//     std::uint64_t const /* nr_time_steps */,
//     std::uint64_t const /* nr_rows */,
//     std::uint64_t const /* nr_cols */,
//     std::uint64_t const /* nr_rows_grain */,
//     std::uint64_t const /* nr_cols_grain */)
// {
//     // TODO Perform some real HPX work
// 
//     using namespace std::chrono_literals;
// 
//     // auto const nr_localities = hpx::get_num_localities().get();
// 
//     // std::this_thread::sleep_for(10s / nr_localities);
// 
//     std::this_thread::sleep_for(10s);
// 
//     return hpx::make_ready_future<double>(5.0);
// }

}  // Anonymous namespace


// HPX_REGISTER_GATHER(double, max_airpolution_gatherer);


namespace lue {

/*!
    @brief      Calculate average airpolution based on synthetic data
*/
void polute_air(
    std::uint64_t const max_tree_depth,
    std::uint64_t const nr_time_steps,
    std::uint64_t const nr_rows,
    std::uint64_t const nr_cols,
    std::uint64_t const nr_rows_partition,
    std::uint64_t const nr_cols_partition)
{
    assert(nr_rows > 0);
    assert(nr_cols > 0);
    assert(nr_rows_partition < nr_rows);
    assert(nr_cols_partition < nr_cols);

    using Shape = lue::Shape<std::uint64_t, 2>;

    Shape area_shape{{nr_rows, nr_cols}};
    Shape partition_shape{{nr_rows_partition, nr_cols_partition}};

    // // Determine which part of the world we need to handle. We need the
    // // location of the grains.
    // auto grains = lue::grains(
    //     area_shape, grain_shape, hpx::get_num_localities(hpx::launch::sync),
    //     hpx::get_locality_id());


    hpx::cout << system_description().get();


    // For each raster input parameter, we need to setup a spatial
    // index with raster partitions. This will make it possible to
    // perform spatial queries.

    








    // auto const locality_id = hpx::get_locality_id();

    // // std::cout << "locality: " << locality_id << std::endl;

    // // TODO Do something useful on the current locality
    // hpx::future<double> local_result = max_airpolution(
    //     nr_time_steps, nr_rows, nr_cols, nr_rows_grain, nr_cols_grain);

    // // TODO Gather results from all localities
    // // When this function is called multiple time (e.g. when
    // // benchmarking), subsequent calls to gather must be made unique by
    // // passing a count argument
    // static int count = 0;

    // if(locality_id == 0) {
    //     // We are the gather destination site
    //     std::vector<double> all_results =
    //         hpx::lcos::gather_here(
    //             gather_basename(), std::move(local_result),
    //             hpx::get_num_localities(hpx::launch::sync), count).get();

    //     assert(!all_results.empty());

    //     // auto const overall_result =
    //     //     *std::max_element(all_results.begin(), all_results.end());

    //     // std::cout << fmt::format(
    //     //     "Received {} results, of which the maximum is {}\n",
    //     //     nr_localities, overall_result);
    // }
    // else {
    //     // We are not the gather source site

    //     // Transmit value to gather destination site (locality_id == 0)
    //     hpx::lcos::gather_there(
    //         gather_basename(), std::move(local_result),
    //         count).wait();
    // }

    // ++count;
}

}  // namespace lue
