#include "change_land_use.hpp"
#include "lue/framework/algorithm.hpp"
#include "lue/framework/core/component/partitioned_array.hpp"
#include "lue/framework/io.hpp"
#include "lue/data_model/hl/raster_view.hpp"
#include <hpx/iostream.hpp>


namespace lue {

    namespace ldm = data_model;
    namespace lh5 = hdf5;


    namespace benchmark {

        void change_land_use(Task const& task, std::size_t const max_tree_depth)
        {
            lue_assert(max_tree_depth > 0);

            using NominalElement = std::int32_t;
            using ScalarElement = double;

            using FloatRaster = PartitionedArray<ScalarElement, 2>;
            using IntegerRaster = PartitionedArray<NominalElement, 2>;
            using RasterShape = Shape<Count, 2>;

            RasterShape raster_shape;
            std::copy(task.array_shape().begin(), task.array_shape().end(), raster_shape.begin());

            RasterShape partition_shape;
            std::copy(task.partition_shape().begin(), task.partition_shape().end(), partition_shape.begin());

            IntegerRaster land_use{raster_shape, partition_shape};
            FloatRaster elevation{raster_shape, partition_shape};

            hpx::cout << describe(land_use) << std::endl;

            lue_assert(land_use.shape() == raster_shape);
            lue_assert(elevation.shape() == raster_shape);

            // Initialize state
            land_use = uniform(land_use, 0, 10);
            elevation = uniform(elevation, 0.0, 10.0);


            // Create a dataset
            std::string const dataset_pathname{"change_land_use.lue"};

            auto dataset_ptr = std::make_shared<ldm::Dataset>(ldm::create_dataset(dataset_pathname));
            using ConstantRasterView = ldm::constant::RasterView<std::shared_ptr<ldm::Dataset>>;
            using VariableRasterView = ldm::variable::RasterView<std::shared_ptr<ldm::Dataset>>;

            // Add raster layers
            std::string const phenomenon_name{"area"};
            std::string const constant_property_set_name{"constant"};
            std::string const variable_property_set_name{"variable"};
            ldm::Clock const clock{ldm::time::Unit::day, 1};

            // In non-Debug builds, lh5::Shape(raster_shape.begin(),
            // raster_shape.end()) results in an undefined symbol error (pointing
            // to std::allocator<unsigned long long>::allocator()). Therefore,
            // this hack.
            lh5::Shape raster_shape_(2);
            std::copy(raster_shape.begin(), raster_shape.end(), raster_shape_.begin());

            ConstantRasterView constant_raster_view{ldm::constant::create_raster_view(
                dataset_ptr,
                phenomenon_name,
                constant_property_set_name,
                // lh5::Shape(raster_shape.begin(), raster_shape.end()),
                raster_shape_,
                {0.0, 0.0, double(raster_shape[1]), double(raster_shape[0])})};
            VariableRasterView variable_raster_view{ldm::variable::create_raster_view(
                dataset_ptr,
                phenomenon_name,
                variable_property_set_name,
                // Also store initial state
                clock,
                task.nr_time_steps() + 1,
                {0, task.nr_time_steps() + 1},
                // lh5::Shape(raster_shape.begin(), raster_shape.end()),
                raster_shape_,
                {0.0, 0.0, double(raster_shape[1]), double(raster_shape[0])})};


            ConstantRasterView::Layer locality_id_layer{
                constant_raster_view.add_layer<std::uint32_t>("locality_id")};
            ConstantRasterView::Layer elevation_layer{
                constant_raster_view.add_layer<ScalarElement>("elevation")};
            VariableRasterView::Layer land_use_layer{
                variable_raster_view.add_layer<NominalElement>("land_use")};


            Count time_step = 0;

            std::vector<hpx::future<void>> written;

            written.push_back(write(locality_id(elevation), locality_id_layer));
            written.push_back(write(elevation, elevation_layer));
            written.push_back(write(land_use, land_use_layer, time_step));

            hpx::sliding_semaphore semaphore{static_cast<std::int64_t>(max_tree_depth)};

            for (++time_step; time_step <= Count(task.nr_time_steps()); ++time_step)
            {

                semaphore.wait(time_step);


                // model ---------------------------------------------------------------
                // No model yet...
                land_use = copy(land_use);

                // /model --------------------------------------------------------------


                written.push_back(write(land_use, land_use_layer, time_step));

                hpx::cout << '.' << std::flush;


                // Attach a continuation to the state at the current time
                // step. Once it is finished, signal the semaphore so it knowns
                // that we can have another iteration in flight.
                hpx::when_all_n(land_use.begin(), land_use.nr_partitions())
                    .then(
                        hpx::launch::sync,
                        [&semaphore, time_step](auto const&) { semaphore.signal(time_step); });
            }


            hpx::cout << "!" << std::flush;

            hpx::wait_all_n(land_use.begin(), land_use.nr_partitions());
            hpx::wait_all_n(written.begin(), written.size());

            hpx::cout << std::endl;
        }

    }  // namespace benchmark
}  // namespace lue
