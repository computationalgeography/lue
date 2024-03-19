#include "lue/framework/io/raster.hpp"
#include "lue/framework/algorithm/create_partitioned_array.hpp"
#include "lue/framework/algorithm/policy.hpp"
#include "lue/framework/core/assert.hpp"
#include "lue/gdal.hpp"
// #include "lue/framework/algorithm/policy/all_values_within_domain.hpp"
// #include "lue/framework/algorithm/policy/default_value_policies.hpp"
#include <hpx/async_colocated/get_colocation_id.hpp>
#include <hpx/async_combinators/when_any.hpp>
#include <hpx/components/get_ptr.hpp>
#include <hpx/shared_mutex.hpp>
#include <memory>


namespace lue {
    namespace policy::read {

        template<typename Element>
        using DefaultPolicies = policy::
            DefaultPolicies<AllValuesWithinDomain<Element>, OutputElements<Element>, InputElements<Element>>;


        template<typename Element>
        using ValuePolicies = policy::DefaultValuePolicies<
            AllValuesWithinDomain<Element>,
            OutputElements<Element>,
            InputElements<Element>>;


        template<typename Element>
        class SpecialValuePolicy:

            public policy::Policies<
                policy::AllValuesWithinDomain<Element>,
                policy::OutputsPolicies<policy::OutputPolicies<
                    policy::DefaultOutputNoDataPolicy<Element>,
                    policy::AllValuesWithinRange<Element, Element>>>,
                policy::InputsPolicies<policy::InputPolicies<policy::DetectNoDataByValue<Element>>>>

        {

            private:

                // MSVC requires that these templates are qualified by their namespaces
                using Base = policy::Policies<
                    policy::AllValuesWithinDomain<Element>,
                    policy::OutputsPolicies<policy::OutputPolicies<
                        policy::DefaultOutputNoDataPolicy<Element>,
                        policy::AllValuesWithinRange<Element, Element>>>,
                    policy::InputsPolicies<policy::InputPolicies<policy::DetectNoDataByValue<Element>>>>;

            public:

                SpecialValuePolicy():

                    SpecialValuePolicy{policy::no_data_value<Element>}

                {
                }


                SpecialValuePolicy(Element const no_data_value):

                    Base{
                        policy::AllValuesWithinDomain<Element>{},
                        policy::OutputPolicies<
                            policy::DefaultOutputNoDataPolicy<Element>,
                            policy::AllValuesWithinRange<Element, Element>>{},
                        policy::InputPolicies<policy::DetectNoDataByValue<Element>>{
                            policy::DetectNoDataByValue<Element>{no_data_value}}}

                {
                }
        };


    }  // namespace policy::read


    namespace {

        class Band
        {

            public:

                using Mutex = hpx::shared_mutex;
                using ReadLock = std::shared_lock<Mutex>;
                using WriteLock = std::unique_lock<Mutex>;


                Band(gdal::RasterBandPtr&& band_ptr):

                    _band_ptr{std::move(band_ptr)}

                {
                }


                Band(Band const&) = delete;


                Band(Band&&) = delete;


                Band& operator=(Band const&) = delete;


                Band& operator=(Band&&) = delete;


                template<typename Element>
                void read_partition(Offset<Index, 2> const& offset, ArrayPartitionData<Element, 2>& data)
                {
                    // Blocks if someone else is writing to the band. Otherwise this does
                    // not block.
                    ReadLock read_lock{_mutex};

                    lue::gdal::Offset gdal_offset{
                        static_cast<lue::gdal::Offset::value_type>(offset[0]),
                        static_cast<lue::gdal::Offset::value_type>(offset[1])};
                    lue::gdal::Shape gdal_shape{
                        static_cast<lue::gdal::Shape::value_type>(data.shape()[0]),
                        static_cast<lue::gdal::Shape::value_type>(data.shape()[1])};

                    lue::gdal::read(
                        *_band_ptr, gdal_offset, gdal_shape, gdal::data_type_v<Element>, data.data());
                }


            private:

                Mutex _mutex;

                gdal::RasterBandPtr _band_ptr;
        };


        using BandPtr = std::shared_ptr<Band>;


        template<typename Element>
        std::tuple<Element, bool> no_data_value(::GDALRasterBand& band)
        {
            return gdal::no_data_value<Element>(band);
        }


        template<typename Element>
        class ReadPartition
        {

            public:

                ReadPartition(gdal::DatasetPtr&& dataset_ptr, BandPtr&& band_ptr):

                    _dataset_ptr{std::move(dataset_ptr)},
                    _band_ptr{std::move(band_ptr)}

                {
                    HPX_ASSERT(_dataset_ptr);
                    HPX_ASSERT(_band_ptr);
                }

                ReadPartition(ReadPartition const&) = default;

                ReadPartition(ReadPartition&&) = delete;

                ~ReadPartition() = default;

                ReadPartition& operator=(ReadPartition const&) = default;

                ReadPartition& operator=(ReadPartition&&) = delete;


                static constexpr Rank rank{2};
                using Partition = ArrayPartition<Element, rank>;
                using Offset = OffsetT<Partition>;
                using Shape = ShapeT<Partition>;

                using Server = typename Partition::Server;
                using Data = DataT<Partition>;


                template<typename InputNoDataPolicy, typename OutputNoDataPolicy>
                Partition instantiate(
                    InputNoDataPolicy const indp,
                    OutputNoDataPolicy const ondp,
                    Offset const& offset,
                    Shape const& shape)
                {
                    Data data{shape};

                    _band_ptr->read_partition(offset, data);

                    std::transform(
                        data.begin(),
                        data.end(),
                        data.begin(),
                        [indp, ondp](Element value)
                        {
                            if (indp.is_no_data(value))
                            {
                                ondp.mark_no_data(value);
                            }

                            return value;
                        });

                    return Partition{hpx::find_here(), offset, std::move(data)};
                }

            private:

                gdal::DatasetPtr _dataset_ptr;

                BandPtr _band_ptr;
        };


        template<typename Element>
        class WritePolicies:

            public policy::Policies<
                policy::AllValuesWithinDomain<Element>,
                policy::OutputsPolicies<policy::OutputPolicies<
                    policy::DefaultOutputNoDataPolicy<Element>,
                    policy::AllValuesWithinRange<Element, Element>>>,
                policy::InputsPolicies<policy::InputPolicies<policy::DefaultInputNoDataPolicy<Element>>>>

        {
        };


        template<typename Policies, typename Partition>
        std::vector<Partition> read_partition_per_locality(
            Policies const& policies,
            std::string const& name,
            std::vector<lue::OffsetT<Partition>> const& offsets,
            std::vector<lue::ShapeT<Partition>> const& partition_shapes)
        {
            gdal::DatasetPtr dataset_ptr{gdal::open_dataset(name, ::GA_ReadOnly)};
            gdal::RasterBandPtr band_ptr{gdal::raster_band(*dataset_ptr)};

            using Element = lue::ElementT<Partition>;

            std::size_t const nr_partitions{std::size(offsets)};
            std::vector<Partition> partitions(nr_partitions);

            auto indp{std::get<0>(policies.inputs_policies()).input_no_data_policy()};
            auto ondp{std::get<0>(policies.outputs_policies()).output_no_data_policy()};

            ReadPartition<Element> partition_reader{
                std::move(dataset_ptr), std::make_shared<Band>(std::move(band_ptr))};

            partitions[0] = hpx::async(

                [indp,
                 ondp,
                 partition_reader,
                 offset = offsets[0],
                 partition_shape = partition_shapes[0]]() mutable
                { return partition_reader.instantiate(indp, ondp, offset, partition_shape); });

            for (std::size_t idx = 1; idx < nr_partitions; ++idx)
            {
                partitions[idx] = partitions[idx - 1].then(

                    [indp,
                     ondp,
                     partition_reader,
                     offset = offsets[idx],
                     partition_shape = partition_shapes[idx]](auto const& /* previous_partition */) mutable
                    { return partition_reader.instantiate(indp, ondp, offset, partition_shape); }

                );
            }

            return partitions;
        }


        template<typename Policies, typename Partition>
        struct ReadPartitionsPerLocalityAction:
            hpx::actions::make_action<
                decltype(&read_partition_per_locality<Policies, Partition>),
                &read_partition_per_locality<Policies, Partition>,
                ReadPartitionsPerLocalityAction<Policies, Partition>>::type
        {
        };


        template<typename Element>
        class ReadPartitionsPerLocality
        {

            public:

                static constexpr Rank rank{2};

                using OutputElement = Element;
                using Partition = lue::ArrayPartition<OutputElement, rank>;
                using Offset = lue::OffsetT<Partition>;
                using Shape = lue::ShapeT<Partition>;

                static constexpr bool instantiate_per_locality{true};


                ReadPartitionsPerLocality(std::string const& name):

                    _name{name}

                {
                }


                template<typename Policies>
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

                        [locality_id,
                         action = std::move(action),
                         policies,
                         name = _name,
                         offsets = std::move(offsets),
                         partition_shapes = std::move(partition_shapes)]()
                        { return action(locality_id, policies, name, offsets, partition_shapes); }

                    );
                }

            private:

                std::string _name;
        };


        template<typename Policies, typename Partition>
        void write_partition(
            [[maybe_unused]] Policies const& policies, std::string const& name, Partition const& partition)
        {
            using Element = ElementT<Partition>;

            lue_hpx_assert(partition.is_ready());

            // Open dataset
            // Open band
            // Write partition's data

            gdal::Raster raster{gdal::open_dataset(name, ::GA_Update)};
            gdal::Raster::Band raster_band{raster.band(1)};

            auto partition_server_ptr{hpx::get_ptr(hpx::launch::sync, partition)};
            auto data{partition_server_ptr->data()};
            auto offset{partition_server_ptr->offset()};

            lue::gdal::Offset gdal_offset{
                static_cast<lue::gdal::Offset::value_type>(offset[0]),
                static_cast<lue::gdal::Offset::value_type>(offset[1])};
            lue::gdal::Shape gdal_shape{
                static_cast<lue::gdal::Shape::value_type>(data.shape()[0]),
                static_cast<lue::gdal::Shape::value_type>(data.shape()[1])};

            raster_band.write(gdal_offset, gdal_shape, gdal::data_type_v<Element>, data.data());
        }


        template<typename Policies, typename Partition>
        struct WritePartitionAction:
            hpx::actions::make_action<
                decltype(&write_partition<Policies, Partition>),
                &write_partition<Policies, Partition>,
                WritePartitionAction<Policies, Partition>>::type
        {
        };

    }  // Anonymous namespace


    template<typename Element>
    class FunctorTraits<ReadPartitionsPerLocality<Element>>
    {
        public:

            static constexpr bool is_functor{true};
    };


    template<typename Element>
    PartitionedArray<Element, 2> read(std::string const& name, Shape<Count, 2> const& partition_shape)
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
        bool no_data_value_is_valid{};
        Element no_data_value;

        {
            gdal::Raster raster{gdal::open_dataset(name, ::GA_ReadOnly)};
            gdal::Raster::Band raster_band{raster.band(1)};

            auto const raster_shape = raster.shape();
            array_shape[0] = static_cast<Count>(raster_shape[0]);
            array_shape[1] = static_cast<Count>(raster_shape[1]);
            std::tie(no_data_value, no_data_value_is_valid) = raster_band.no_data_value<Element>();
        }

        using Functor = ReadPartitionsPerLocality<Element>;

        if (!no_data_value_is_valid)
        {
            // No no-data value is set in the band. Don't bother with no-data.

            using Policies = policy::read::DefaultPolicies<Element>;
            Policies policies{};
            return create_partitioned_array(policies, array_shape, partition_shape, Functor{name});
        }
        else
        {
            // A no-data value is set in the band. Let's be nice and use it. Upon reading,
            // it will be converted to our own conventions for handling no-data.

            if (std::is_floating_point_v<Element> && std::isnan(no_data_value))
            {
                // No-data value is a NaN. This is what LUE uses as no-data for floating point
                // by default.
                using Policies = policy::read::ValuePolicies<Element>;
                Policies policies{};
                return create_partitioned_array(policies, array_shape, partition_shape, Functor{name});
            }
            else
            {
                // No-data value is not NaN. Use a special no-data policy for this special value.
                using Policies = policy::read::SpecialValuePolicy<Element>;
                Policies policies{no_data_value};
                return create_partitioned_array(policies, array_shape, partition_shape, Functor{name});
            }
        }
    }


    template<typename Element>
    hpx::future<void> write(
        PartitionedArray<Element, 2> const& array, std::string const& name, std::string const& clone_name)
    {
        using Policies = WritePolicies<Element>;
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

        Policies policies{};
        auto const& ondp = std::get<0>(policies.outputs_policies()).output_no_data_policy();
        Element no_data_value;
        ondp.mark_no_data(no_data_value);

        {
            Count const nr_bands{1};
            GDALDataType const data_type{gdal::data_type_v<Element>};
            gdal::DatasetPtr dataset_ptr{gdal::create_dataset(
                "GTiff",
                name,
                gdal::Shape{
                    static_cast<gdal::Count>(array.shape()[0]), static_cast<gdal::Count>(array.shape()[1])},
                nr_bands,
                data_type)};
            gdal::RasterBandPtr band_ptr{gdal::raster_band(*dataset_ptr)};

            gdal::set_no_data_value(*band_ptr, no_data_value);

            if (!clone_name.empty())
            {
                // Copy stuff from clone dataset

                gdal::DatasetPtr clone_dataset_ptr{gdal::open_dataset(clone_name, ::GA_ReadOnly)};

                Shape<Count, 2> const shape{
                    clone_dataset_ptr->GetRasterYSize(), clone_dataset_ptr->GetRasterXSize()};

                if (shape != array.shape())
                {
                    throw std::runtime_error("Shapes of clone raster and raster to write differ");
                }

                double geo_transform[6];

                if (clone_dataset_ptr->GetGeoTransform(geo_transform) == CE_None)
                {
                    dataset_ptr->SetGeoTransform(geo_transform);
                }

                if (OGRSpatialReference const* spatial_reference = clone_dataset_ptr->GetSpatialRef())
                {
                    dataset_ptr->SetSpatialRef(spatial_reference);
                }
            }
        }

        // Asynchronously spawn a task that will write each ready partition to the dataset,
        // one after the other. This task stops once all partitions have been written to the dataset.

        std::vector<Partition> partitions(array.partitions().begin(), array.partitions().end());

        hpx::future<void> result = hpx::async(
            [name, partitions = std::move(partitions)]() mutable
            {
                using Action = WritePartitionAction<Policies, Partition>;

                std::size_t nr_partitions_to_write{partitions.size()};
                Action action{};
                hpx::when_any_result<std::vector<Partition>> when_any_result;
                std::size_t idx;

                while (nr_partitions_to_write > 0)
                {
                    // Find a ready partition. Wait for it if necessary.
                    when_any_result =
                        hpx::when_any(partitions.begin(), partitions.begin() + nr_partitions_to_write).get();
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
            });

        return result;
    }

}  // namespace lue


#define INSTANTIATE(type)                                                                                    \
                                                                                                             \
    template lue::PartitionedArray<type, 2> lue::read<type>(                                                 \
        std::string const&, lue::Shape<lue::Count, 2> const&);                                               \
                                                                                                             \
    template hpx::future<void> lue::write<type>(                                                             \
        lue::PartitionedArray<type, 2> const&, std::string const&, std::string const&);


INSTANTIATE(uint8_t)
INSTANTIATE(uint32_t)
INSTANTIATE(int32_t)
#if LUE_GDAL_SUPPORTS_64BIT_INTEGERS
INSTANTIATE(uint64_t)
INSTANTIATE(int64_t)
#endif
INSTANTIATE(float)
INSTANTIATE(double)


#undef INSTANTIATE
