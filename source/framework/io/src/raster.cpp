#include "lue/framework/io/raster.hpp"
#include "lue/framework/core/assert.hpp"
#include "lue/framework/io/gdal.hpp"
#include "lue/framework/algorithm/create_partitioned_array.hpp"
#include "lue/framework/algorithm/policy/all_values_within_domain.hpp"
#include "lue/framework/algorithm/policy/default_value_policies.hpp"
#include <hpx/async_colocated/get_colocation_id.hpp>
#include <hpx/async_combinators/when_any.hpp>
#include <hpx/components/get_ptr.hpp>
#include <memory>


/// #include "pcc/io.hpp"
/// #include "pcc/annotate.hpp"
/// #include "pcc/configuration.hpp"
/// #include "pcc/wait_if.hpp"
/// #include <hpx/iostream.hpp>
/// #include <hpx/shared_mutex.hpp>


namespace lue {
    namespace {

///         template<
///             typename Instantiator>
///         Partitions read_partitions(
///             Shape const& shape,
///             Shape const& partition_shape,
///             Instantiator&& instantiator)
///         {
///             // Each next partition is only created once the previous one has been created. It
///             // does not make sense to read partitions in parallel.
/// 
///             if(partition_shape > shape)
///             {
///                 throw std::runtime_error(
///                         "Partition shape must be smaller than array shape: " +
///                         shape_to_string(partition_shape) + " < " + shape_to_string(shape)
///                     );
///             }
/// 
///             Partitions result{};
/// 
///             if(nr_elements(shape) > 0 && nr_elements(partition_shape) > 0)
///             {
///                 Shape const shape_in_partitions{
///                         shape[0] / partition_shape[0],
///                         shape[1] / partition_shape[1]
///                     };
///                 Count const nr_partitions{nr_elements(shape_in_partitions)};
///                 Partitions::Collection collection(nr_partitions);
/// 
///                 if(shape_in_partitions[0] < 2 || shape_in_partitions[1] < 2)
///                 {
///                     throw std::runtime_error("Shape in partitions must be larger or equal to (2, 2)");
///                 }
/// 
///                 auto instantiate =
///                     [instantiator](
///                         Offset const& offset,
///                         Shape const& shape) mutable
///                 {
///                     Annotation{"read"};
/// 
///                     return instantiator.instantiate(offset, shape);
///                 };
/// 
///                 {
///                     Index p{0};
///                     Index cell_idx0{0};
///                     Index cell_idx1{0};
///                     Shape new_partition_shape{};
/// 
///                     // Partitions in all rows, except for the last one
///                     for(Index partition_idx0 = 0; partition_idx0 < shape_in_partitions[0] - 1; ++partition_idx0)
///                     {
///                         // Partitions in all columns, except for the last one
///                         for(Index partition_idx1 = 0; partition_idx1 < shape_in_partitions[1] - 1; ++partition_idx1)
///                         {
///                             if(p == 0)
///                             {
///                                 // First partition
///                                 HPX_ASSERT(p < Count(std::size(collection)));
///                                 HPX_ASSERT(!collection[p].valid());
///                                 collection[p] = hpx::async(
///                                     instantiate, Offset{cell_idx0, cell_idx1}, partition_shape);
///                             }
///                             else
///                             {
///                                 HPX_ASSERT(p > 0 && p < Count(std::size(collection)));
///                                 HPX_ASSERT(collection[p - 1].valid());
///                                 HPX_ASSERT(!collection[p].valid());
///                                 collection[p] = collection[p - 1].then(
///                                     [instantiate, cell_idx0, cell_idx1, partition_shape](
///                                         [[maybe_unused]] hpx::shared_future<Partition> const& partition_ftr) mutable
///                                     {
///                                         return instantiate(Offset{cell_idx0, cell_idx1}, partition_shape);
///                                     });
///                             }
/// 
///                             ++p;
///                             cell_idx1 += partition_shape[1];
///                         }
/// 
///                         // Partition in last column of current row
///                         new_partition_shape = Shape{partition_shape[0], shape[1] - cell_idx1};
/// 
///                         HPX_ASSERT(new_partition_shape[0] >= partition_shape[0]);
///                         HPX_ASSERT(new_partition_shape[1] >= partition_shape[1]);
/// 
///                         collection[p] = collection[p - 1].then(
///                             [instantiate, cell_idx0, cell_idx1, new_partition_shape](
///                                 [[maybe_unused]] hpx::shared_future<Partition> const& partition_ftr) mutable
///                             {
///                                 return instantiate(Offset{cell_idx0, cell_idx1}, new_partition_shape);
///                             });
///                         ++p;
///                         cell_idx0 += partition_shape[0];
///                         cell_idx1 = 0;
///                     }
/// 
///                     // Partitions in last row, except for last column
///                     for(Index partition_idx1 = 0; partition_idx1 < shape_in_partitions[1] - 1; ++partition_idx1)
///                     {
///                         new_partition_shape = Shape{shape[0] - cell_idx0, partition_shape[1]};
/// 
///                         HPX_ASSERT(new_partition_shape[0] >= partition_shape[0]);
///                         HPX_ASSERT(new_partition_shape[1] >= partition_shape[1]);
/// 
///                         HPX_ASSERT(p > 0 && p < Count(std::size(collection)));
///                         HPX_ASSERT(collection[p - 1].valid());
///                         HPX_ASSERT(!collection[p].valid());
/// 
///                         collection[p] = collection[p - 1].then(
///                             [instantiate, cell_idx0, cell_idx1, new_partition_shape](
///                                 [[maybe_unused]] hpx::shared_future<Partition> const& partition_ftr) mutable
///                             {
///                                 return instantiate(Offset{cell_idx0, cell_idx1}, new_partition_shape);
///                             });
///                         ++p;
///                         cell_idx1 += partition_shape[1];
///                     }
/// 
///                     // Partition in last row and last column
///                     new_partition_shape = Shape{shape[0] - cell_idx0, shape[1] - cell_idx1};
/// 
///                     HPX_ASSERT(new_partition_shape[0] >= partition_shape[0]);
///                     HPX_ASSERT(new_partition_shape[1] >= partition_shape[1]);
/// 
///                     HPX_ASSERT(p > 0 && p < Count(std::size(collection)));
///                     HPX_ASSERT(collection[p - 1].valid());
///                     HPX_ASSERT(!collection[p].valid());
/// 
///                     collection[p] = collection[p - 1].then(
///                         [instantiate, cell_idx0, cell_idx1, new_partition_shape](
///                             [[maybe_unused]] hpx::shared_future<Partition> const& partition_ftr) mutable
///                         {
///                             return instantiate(Offset{cell_idx0, cell_idx1}, new_partition_shape);
///                         });
///                     ++p;
/// 
///                     HPX_ASSERT(p == nr_partitions);
///                     HPX_ASSERT(cell_idx0 + new_partition_shape[0] == shape[0]);
///                     HPX_ASSERT(cell_idx1 + new_partition_shape[1] == shape[1]);
///                 }
/// 
///                 result = Partitions{std::move(collection), shape_in_partitions};
///             }
/// 
///             return result;
///         }



        /// // This pointer is shared and the object pointed to must be closed once the last
        /// // pointer instance goes out of scope
        /// using GDALDatasetPtr = std::shared_ptr<::GDALDataset>;

        /// // The object pointed to is owned by the dataset and must not be deleted
        /// using GDALBandPtr = ::GDALRasterBand*;

        /// using GDALDriverPtr = ::GDALDriver*;


        class Band
        {

            public:

                using Mutex = hpx::shared_mutex;
                using ReadLock = std::shared_lock<Mutex>;
                using WriteLock = std::unique_lock<Mutex>;


                Band(GDALBandPtr&& band_ptr):

                    _band_ptr{std::move(band_ptr)}

                {
                }


                Band(Band const&)=delete;


                Band(Band&&)=delete;


                Band& operator=(Band const&)=delete;


                Band& operator=(Band&&)=delete;


                template<
                    typename Element>
                void read_partition(
                    // server::ArrayPartition<Element, 2>& partition)
                    Offset<Index, 2> const& offset,
                    ArrayPartitionData<Element, 2>& data)
                {
                    // Blocks if someone else is writing to the band. Otherwise this does
                    // not block.
                    ReadLock read_lock{_mutex};

                    // CPLErr const status{
                    //         _band_ptr->RasterIO(
                    //                 GF_Read,
                    //                 partition.offset()[1], partition.offset()[0],
                    //                 partition.shape()[1], partition.shape()[0],
                    //                 partition.data().data(),
                    //                 partition.shape()[1], partition.shape()[0],
                    //                 GDALTypeTraits<Element>::type_id,
                    //                 0, 0, nullptr
                    //             )
                    //     };

                    CPLErr const status{
                            _band_ptr->RasterIO(
                                    GF_Read,
                                    offset[1], offset[0],
                                    data.shape()[1], data.shape()[0],
                                    data.data(),
                                    data.shape()[1], data.shape()[0],
                                    GDALTypeTraits<Element>::type_id,
                                    0, 0, nullptr
                                )
                        };

                    if(status != CE_None)
                    {
                        throw std::runtime_error("Cannot read partition data");
                    }
                }


                // void write_partition(
                //     Partition const& partition)
                // {
                //     // Blocks if someone else is writing to the band.
                //     WriteLock write_lock{_mutex};

                //     Annotation{"write"};

                //     CPLErr const status{
                //             _band_ptr->RasterIO(
                //                     GF_Write,
                //                     partition.offset()[1], partition.offset()[0],
                //                     partition.shape()[1], partition.shape()[0],
                //                     const_cast<Partition&>(partition).data(),
                //                     partition.shape()[1], partition.shape()[0], GDT_Float64,
                //                     0, 0, nullptr
                //                 )
                //         };

                //     if(status != CE_None)
                //     {
                //         throw std::runtime_error("Cannot write partition data");
                //     }
                // }


            private:

                Mutex _mutex;

                GDALBandPtr _band_ptr;

        };


        using BandPtr = std::shared_ptr<Band>;


        template<
            typename Element>
        Element no_data_value(
            ::GDALRasterBand& band,
            int* success)
        {
            return band.GetNoDataValue(success);
        }


 #if GDAL_VERSION_NUM >= GDAL_COMPUTE_VERSION(3, 5, 0)
        template<>
        std::int64_t no_data_value(
            ::GDALRasterBand& band,
            int* success)
        {
            return band.GetNoDataValueAsInt64(success);
        }


        template<>
        std::uint64_t no_data_value(
            ::GDALRasterBand& band,
            int* success)
        {
            return band.GetNoDataValueAsUInt64(success);
        }
#endif


        template<
            typename Element>
        Element no_data_value(
            ::GDALRasterBand& band)
        {
            int success;

            Element value = no_data_value<Element>(band, &success);

            if(success == 0)
            {
                value = lue::policy::no_data_value<Element>;
            }

            return value;
        }


        template<
            typename Element>
        class ReadPartition
        {

            public:

                ReadPartition(
                    GDALDatasetPtr&& dataset_ptr,
                    BandPtr&& band_ptr):

                    _dataset_ptr{std::move(dataset_ptr)},
                    _band_ptr{std::move(band_ptr)}

                {
                    HPX_ASSERT(_dataset_ptr);
                    HPX_ASSERT(_band_ptr);
                }

                ReadPartition(ReadPartition const&)=default;

                ReadPartition(ReadPartition&&)=delete;

                ~ReadPartition()=default;

                ReadPartition& operator=(ReadPartition const&)=default;

                ReadPartition& operator=(ReadPartition&&)=delete;


                static constexpr Rank rank{2};
                using Partition = ArrayPartition<Element, rank>;
                using Offset = OffsetT<Partition>;
                using Shape = ShapeT<Partition>;

                using Server = typename Partition::Server;
                using Data = DataT<Partition>;


                Partition instantiate(
                    Offset const& offset,
                    Shape const& shape)
                {
                    // Server server{offset, shape};

                    // _band_ptr->read_partition(server);

                    // return Partition{server.get_id()};

                    Data data{shape};

                    _band_ptr->read_partition(offset, data);

                    return Partition{hpx::find_here(), offset, std::move(data)};
                }

            private:

                GDALDatasetPtr _dataset_ptr;

                BandPtr _band_ptr;

        };


///         hpx::future<void> write_partitions(
///             Partitions const& partitions,
///             GDALDatasetPtr&& dataset_ptr,
///             BandPtr&& band_ptr)
///         {
///             // Attach a continuation to each partition, which will write out the partition's
///             // data once it is ready. Use a mutex to prevent multiple partitions from being written
///             // at the same time.
/// 
///             std::vector<hpx::future<void>> written(partitions.nr_partitions());
/// 
///             for(Index p = 0; p < partitions.nr_partitions(); ++p)
///             {
///                 written[p] = partitions(p).then(
///                         [dataset_ptr, band_ptr](
///                             hpx::shared_future<Partition> const& partition)
///                         {
///                             band_ptr->write_partition(partition.get());
///                         }
///                     );
///             }
/// 
///             return hpx::when_all(written);
///         }


        template<
            typename Element>
        class ReadPolicies:

            public policy::Policies<
                    policy::AllValuesWithinDomain<Element>,
                    policy::OutputsPolicies<
                            policy::OutputPolicies<
                                    policy::DefaultOutputNoDataPolicy<Element>,
                                    policy::AllValuesWithinRange<Element, Element>
                                >
                        >,
                    policy::InputsPolicies<
                            policy::InputPolicies<
                                    policy::DefaultInputNoDataPolicy<Element>
                                >
                        >
                >

        {
        };


        template<
            typename Element>
        class WritePolicies:

            public policy::Policies<
                    policy::AllValuesWithinDomain<Element>,
                    policy::OutputsPolicies<
                            policy::OutputPolicies<
                                    policy::DefaultOutputNoDataPolicy<Element>,
                                    policy::AllValuesWithinRange<Element, Element>
                                >
                        >,
                    policy::InputsPolicies<
                            policy::InputPolicies<
                                    policy::DefaultInputNoDataPolicy<Element>
                                >
                        >
                >

        {
        };


        /// template<
        ///     typename Partition,
        ///     typename Policies>
        /// Partition read_partition(
        ///     Policies const& policies,
        ///     GDALBandPtr const& band_ptr,
        ///     OffsetT<Partition> const& offset,
        ///     ShapeT<Partition> const& partition_shape)
        /// {
        ///     using Server = typename Partition::Server;
        ///     using Data = typename Partition::Data;

        ///     Data data{partition_shape};

        ///     // Element*       data()

        ///     Server server{offset, std::move(data)};

        ///     return Partition{server.get_id()};
        /// }


        template<
            typename Policies,
            typename Partition>
        std::vector<Partition> read_partition_per_locality(
            Policies const& policies,
            std::string const& name,
            std::vector<lue::OffsetT<Partition>> const& offsets,
            std::vector<lue::ShapeT<Partition>> const& partition_shapes)
        {
            register_gdal_drivers();  // On all localities

            GDALDatasetPtr dataset_ptr{open_dataset(name, ::GA_ReadOnly)};
            GDALBandPtr band_ptr{get_raster_band(*dataset_ptr)};

            using Element = lue::ElementT<Partition>;

            std::size_t const nr_partitions{std::size(offsets)};
            std::vector<Partition> partitions(nr_partitions);

            ReadPartition<Element> partition_reader{
                    std::move(dataset_ptr),
                    std::make_shared<Band>(std::move(band_ptr))
                };

            partitions[0] = hpx::async(

                    [policies, partition_reader, offset=offsets[0], partition_shape=partition_shapes[0]]() mutable
                    {
                        return partition_reader.instantiate(offset, partition_shape);
                    }
                );

            for(std::size_t idx = 1; idx < nr_partitions; ++idx)
            {
                partitions[idx] = partitions[idx-1].then(

                        [policies, partition_reader, offset=offsets[idx], partition_shape=partition_shapes[idx]](
                            auto const& /* previous_partition */) mutable
                        {
                            return partition_reader.instantiate(offset, partition_shape);
                        }

                    );
            }

            return partitions;
        }


        template<
            typename Policies,
            typename Partition>
        struct ReadPartitionsPerLocalityAction:
            hpx::actions::make_action<
                    decltype(&read_partition_per_locality<Policies, Partition>),
                    &read_partition_per_locality<Policies, Partition>,
                    ReadPartitionsPerLocalityAction<Policies, Partition>
                >::type
        {};


        template<
            typename Element>
        class ReadPartitionsPerLocality
        {

            public:

                static constexpr Rank rank{2};

                using OutputElement = Element;
                using Partition = lue::ArrayPartition<OutputElement, rank>;
                using Offset = lue::OffsetT<Partition>;
                using Shape = lue::ShapeT<Partition>;

                static constexpr bool instantiate_per_locality{true};


                ReadPartitionsPerLocality(
                    std::string const& name):

                    _name{name}

                {
                }


                template<
                    typename Policies>
                hpx::future<std::vector<Partition>> instantiate(
                    hpx::id_type const locality_id,
                    [[maybe_unused]] Policies const& policies,
                    [[maybe_unused]] Shape const& array_shape,
                    std::vector<Offset> offsets,
                    std::vector<Shape> partition_shapes)
                {
                    using Action = ReadPartitionsPerLocalityAction<Policies, Partition>;
                    Action action{};

                    return hpx::async(

                            [locality_id, action=std::move(action), policies, name=_name,
                                offsets=std::move(offsets), partition_shapes=std::move(partition_shapes)]()
                            {
                                return action(locality_id, policies, name, offsets, partition_shapes);
                            }

                        );
                }

            private:

                std::string _name;

        };


        // template<
        //     typename Element,
        //     Rank rank>
        // class FunctorTraits<
        //     NumberPartitionsPerLocality<Element, rank>>
        // {

        //     public:

        //         static constexpr bool const is_functor{true};

        // };


        template<
            typename Policies,
            typename Partition>
        void write_partition(
            [[maybe_unused]] Policies const& policies,
            std::string const& name,
            Partition const& partition)
        {
            using Element = ElementT<Partition>;

            register_gdal_drivers();  // On all localities

            lue_hpx_assert(partition.is_ready());

            // Open dataset
            // Open band
            // Write partition's data

            GDALDatasetPtr dataset_ptr{open_dataset(name, ::GA_Update)};
            GDALBandPtr band_ptr{get_raster_band(*dataset_ptr)};

            auto partition_server_ptr{hpx::get_ptr(hpx::launch::sync, partition)};
            auto data{partition_server_ptr->data()};
            auto offset{partition_server_ptr->offset()};

            CPLErr const status{
                    band_ptr->RasterIO(
                            GF_Write,
                            offset[1], offset[0],
                            data.shape()[1], data.shape()[0],
                            data.data(),
                            data.shape()[1], data.shape()[0],
                            GDALTypeTraits<Element>::type_id,
                            0, 0, nullptr
                        )
                };

            if(status != CE_None)
            {
                throw std::runtime_error("Cannot write partition data");
            }
        }


        template<
            typename Policies,
            typename Partition>
        struct WritePartitionAction:
            hpx::actions::make_action<
                    decltype(&write_partition<Policies, Partition>),
                    &write_partition<Policies, Partition>,
                    WritePartitionAction<Policies, Partition>
                >::type
        {};

    }  // Anonymous namespace


    template<
        typename Element>
    PartitionedArray<Element, 2> read(
        std::string const& name,
        Shape<Count, 2> const& partition_shape)
    {
        // Create a new partitioned array. Each partition is filled with the corresponding
        // subset of raster cells found in the raster pointed to by the name passed in.
        // Each process is allowed to read partitions concurrently. Within each process,
        // partitions are read one after the other.
        // Each partition read can already participate in subsequent calculations. This results
        // in subsequent I/O being hidden by calculations.
        // Even though I/O is always relatively slow, and reading relatively small partitions
        // is even slower, I/O hidden by computations is free.

        // On the / this root locality:
        // - Determine shape of the array to create
        // - Determine the no-data value to use
        // Only then can we call the logic that creates a partitioned array asynchronously.

        Shape<Count, 2> array_shape;
        Element no_data_value;

        {
            register_gdal_drivers();  // On root locality

            GDALDatasetPtr dataset_ptr{open_dataset(name, ::GA_ReadOnly)};

            array_shape[0] = dataset_ptr->GetRasterYSize();
            array_shape[1] = dataset_ptr->GetRasterXSize();

            GDALBandPtr band_ptr{get_raster_band(*dataset_ptr)};

            no_data_value = lue::no_data_value<Element>(*band_ptr);
        }

        // TODO Iff Element is floating point, assume input no-data policy is
        //      DetectNoDataByNaN<Element>, else DetectNoDataByValue<Element> and pass correct value.
        //      If (later) Element is floating point, and input no-data turns out to be not
        //      NaN in some cases, create a new policy that works in case input no-data is NaN
        //      or some value.
        using Policies = ReadPolicies<Element>;
        using Functor = ReadPartitionsPerLocality<Element>;

        return create_partitioned_array(Policies{}, array_shape, partition_shape, Functor{name});
    }


    template<
        typename Element>
    hpx::future<void> write(
        PartitionedArray<Element, 2> const& array,
        std::string const& name,
        std::string const& clone_name)
    {
        using Array = PartitionedArray<Element, 2>;
        using Partition = PartitionT<Array>;

        // Write a partitioned array to a raster band using the GDAL API.
        // All localities containing partitions are iterated over. For each partition within
        // a locality for which the data is ready, the data will be written. Only for
        // one ready partition will the data be written at the same time. This requires some
        // synchronization:
        // - Iterate over all localities. Only move to next locality once all partitions within
        //   a locality are written.
        // - Within a locality, write all partitions.

        // On the / this root locality:
        // - Create the dataset to write a band in

        {
            register_gdal_drivers();  // On root locality

            GDALDatasetPtr clone_dataset_ptr{open_dataset(clone_name, ::GA_ReadOnly)};

            Shape<Count, 2> const shape{
                clone_dataset_ptr->GetRasterYSize(), clone_dataset_ptr->GetRasterXSize()};

            if(shape != array.shape())
            {
                throw std::runtime_error("Shapes of clone raster and raster to write differ");
            }

            Count const nr_bands{1};
            GDALDataType const data_type{GDALTypeTraits<Element>::type_id};
            GDALDatasetPtr dataset_ptr{create(name, shape, nr_bands, data_type)};

            double geo_transform[6];

            if(clone_dataset_ptr->GetGeoTransform(geo_transform) == CE_None)
            {
                dataset_ptr->SetGeoTransform(geo_transform);
            }

            if(OGRSpatialReference const* spatial_reference = clone_dataset_ptr->GetSpatialRef())
            {
                dataset_ptr->SetSpatialRef(spatial_reference);
            }

            // TODO Configure no-data value
        }

        // Asynchronously spawn a task that will write each ready partition to the dataset,
        // one after the other. This task stops once all partitions have been written to the dataset.

        std::vector<Partition> partitions(array.partitions().begin(), array.partitions().end());

        hpx::future<void> result = hpx::async(
                [name, partitions=std::move(partitions)]() mutable
                {
                    using Policies = WritePolicies<Element>;
                    using Action = WritePartitionAction<Policies, Partition>;

                    std::size_t nr_partitions_to_write{partitions.size()};
                    Action action{};
                    hpx::when_any_result<std::vector<Partition>> when_any_result;
                    std::size_t idx;

                    while(nr_partitions_to_write > 0)
                    {
                        // Find a ready partition. Wait for it if necessary.
                        when_any_result = hpx::when_any(
                            partitions.begin(), partitions.begin() + nr_partitions_to_write).get();
                        partitions = std::move(when_any_result.futures);
                        idx = when_any_result.index;

                        // Write the ready partition to the dataset. Wait for it to finish.
                        hpx::id_type const locality{
                            hpx::get_colocation_id(hpx::launch::sync, partitions[idx].get_id())};
                        action(locality, Policies{}, name, partitions[idx]);

                        // Move the ready and written partition to just after the range with still
                        // not written partitions
                        std::rotate(
                            partitions.begin() + idx,
                            partitions.begin() + idx + 1,
                            partitions.begin() + nr_partitions_to_write);

                        --nr_partitions_to_write;
                    }
                }
            );

        return result;


///         GDALBandPtr band_ptr{get_raster_band(*dataset_ptr)};
/// 
///         hpx::future<void> result{write_partitions(
///                 raster.partitions(),
///                 std::move(dataset_ptr),
///                 std::make_shared<Band>(std::move(band_ptr))
///             )};
    }

}  // namespace lue


#define INSTANTIATE(type)                               \
                                                        \
    template lue::PartitionedArray<type, 2> lue::read(  \
        std::string const&,                             \
        lue::Shape<lue::Count, 2> const&);              \
                                                        \
    template hpx::future<void> lue::write(              \
        lue::PartitionedArray<type, 2> const&,          \
        std::string const&, std::string const&);


    INSTANTIATE(uint8_t)
    INSTANTIATE(uint32_t)
    INSTANTIATE(int32_t)
#if GDAL_VERSION_NUM >= GDAL_COMPUTE_VERSION(3, 5, 0)
    INSTANTIATE(uint64_t)
    INSTANTIATE(int64_t)
#endif
    INSTANTIATE(float)
    INSTANTIATE(double)


#undef INSTANTIATE
