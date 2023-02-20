#include "burn_wilderness.hpp"
#include "wildfire_model.hpp"
#include "lue/framework/model/simulate.hpp"


namespace lue {
    namespace benchmark {

        void burn_wilderness(Task const& task, std::size_t const /* max_tree_depth */)
        {
            // lue_assert(max_tree_depth > 0);


            // Given the arguments passed in, instantiate the model and simulate
            // the process

            WildfireModel::RasterShape raster_shape;
            WildfireModel::RasterShape partition_shape;

            std::copy(task.array_shape().begin(), task.array_shape().end(), raster_shape.begin());
            std::copy(task.partition_shape().begin(), task.partition_shape().end(), partition_shape.begin());

            WildfireModel model{task.nr_time_steps(), raster_shape, partition_shape};

            simulate_process(model, task.nr_time_steps());
        }

    }  // namespace benchmark
}  // namespace lue
