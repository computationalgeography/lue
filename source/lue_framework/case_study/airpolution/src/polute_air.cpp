#include "polute_air.hpp"
#include <hpx/config.hpp>
#include "lue/data_model.hpp"
#include <hpx/runtime.hpp>


namespace lue {

void polute_air(
    std::string const& dataset_pathname)
{
    // We are running on a single locality. Determine which part of
    // the raster to calculate the airpolution for, based on the id of our
    // locality and the total number of localities.

    // t threads are distributed over l localities

    // Since the model only contains local operations, the shape of the
    // world each locality selects is not relevant. It can be whatever
    // is optimal given the layout of the data in the dataset.

    // The current locality
    auto const locality = hpx::get_locality_id();

    // The sum of all threads in all localities
    auto const nr_worker_threads = hpx::get_num_worker_threads();

    auto const nr_localities = hpx::get_num_localities();



    // Find input raster in dataset. Extent + discretization determine the
    // shape of the raster.
    Dataset const dataset{dataset_pathname};

    // ...




}

}  // namespace lue
