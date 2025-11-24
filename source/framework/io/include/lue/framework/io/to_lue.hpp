#pragma once
#include "lue/framework/algorithm/policy.hpp"
#include "lue/framework/core/annotate.hpp"
#include "lue/framework/core/component.hpp"
#include "lue/framework/io/configure.hpp"
#include "lue/framework/io/dataset.hpp"
#include "lue/framework/io/util.hpp"
#include "lue/data_model/hl/util.hpp"


/*!
    @file

    Thiѕ header contains the implementation of the various to_lue function overloads.

    The goal is to return an array as soon as possible, allowing the caller to create more work, while
    the write-tasks start the writing.
*/


namespace lue {
    namespace detail {

        template<typename Policies, typename Partition, typename CreateHyperslab, typename Property>
        void write_partition(
            [[maybe_unused]] Policies const& policies,
            Partition const& partition,
            CreateHyperslab create_hyperslab,
            data_model::ID const object_id,
            Property& property)
        {
            AnnotateFunction const annotate{"to_lue: partition"};

            // Open value. Configure for use of parallel I/O if necessary.
            hdf5::Dataset::TransferPropertyList transfer_property_list{};

#ifdef LUE_FRAMEWORK_WITH_PARALLEL_IO
            transfer_property_list.set_transfer_mode(H5FD_MPIO_INDEPENDENT);
#endif

            auto& value{property.value()};
            auto array{value[object_id]};

            using Element = ElementT<Partition>;

            auto partition_ptr{detail::ready_component_ptr(partition)};
            auto& partition_server{*partition_ptr};
            Element* buffer{partition_server.data().data()};

            array.write(create_hyperslab(partition_server), transfer_property_list, buffer);
        }


        template<typename Policies, typename Partitions>
        auto write_partitions_constant(
            Policies const& policies,
            hdf5::Offset const& array_hyperslab_start,  // Only needed to offset block written to array
            Partitions const& partitions,
            std::string const& array_pathname,
            data_model::ID const object_id) -> hpx::future<void>
        {
            auto const [dataset_pathname, phenomenon_name, property_set_name, property_name] =
                parse_array_pathname(array_pathname);

            // TODO: How to make all I/O happen on the I/O thread? Passing executor to continuation works in
            // parallel I/O case but serial case hangs. Currently, the nested I/O ends up on the compute
            // thread it seems.
            // hpx::execution::experimental::io_pool_executor executor;

            return hpx::when_any(partitions)
                .then(
                    // executor,
                    [  // executor,
                        policies,
                        array_hyperslab_start,
                        dataset_pathname,
                        phenomenon_name,
                        property_set_name,
                        property_name,
                        object_id](auto when_any_result_f)
                    {
                        AnnotateFunction const annotate{"to_lue: partitions constant"};

                        // Open the dataset once the first partition is ready to be written
                        auto dataset = open_dataset(dataset_pathname, H5F_ACC_RDWR);

                        // Now that dataset is open, write the ready partition to it

                        // Open phenomenon
                        auto& phenomenon{dataset.phenomena()[phenomenon_name]};

                        // Open property-set
                        auto& property_set{phenomenon.property_sets()[property_set_name]};

                        // Open property
                        lue_hpx_assert(property_set.properties().contains(property_name));
                        lue_hpx_assert(
                            property_set.properties().shape_per_object(property_name) ==
                            data_model::ShapePerObject::different);
                        lue_hpx_assert(
                            property_set.properties().value_variability(property_name) ==
                            data_model::ValueVariability::constant);
                        using Properties = data_model::different_shape::Properties;
                        auto& property{property_set.properties().collection<Properties>()[property_name]};

                        using Partition = typename Partitions::value_type;
                        using PartitionServer = Partition::Server;

                        auto create_hyperslab =
                            [array_hyperslab_start](PartitionServer const& partition_server)
                        { return hyperslab(array_hyperslab_start, partition_server); };

                        auto [partition_idx, partitions] = when_any_result_f.get();

                        auto write_partition = detail::write_partition<
                            std::remove_reference_t<std::remove_cv_t<Policies>>,
                            std::remove_reference_t<std::remove_cv_t<Partition>>,
                            std::remove_reference_t<std::remove_cv_t<decltype((create_hyperslab))>>,
                            std::remove_reference_t<std::remove_cv_t<decltype((property))>>>;

                // std::vector<hpx::future<void>> partitions_written{};
                // partitions_written.reserve(partitions.size());
                // partitions_written.emplace_back(
                //     hpx::async(
                //         // executor,
                //         write_partition,
                //         std::ref(policies),
                //         std::ref(partitions[partition_idx]),
                //         create_hyperslab,
                //         object_id,
                //         std::ref(property)));

#ifndef HDF5_IS_THREADSAFE
#else
#endif

                        hpx::async(
                            // executor,
                            write_partition,
                            std::ref(policies),
                            std::ref(partitions[partition_idx]),
                            create_hyperslab,
                            object_id,
                            std::ref(property))
                            .get();

                        partitions.erase(partitions.begin() + partition_idx);

                        // Write any other ready partition, until all partitions are written, one after the
                        // other
                        while (!partitions.empty())
                        {
                            auto [partition_idx, partitions_] = hpx::when_any(partitions).get();

                            hpx::async(
                                // executor,
                                write_partition,
                                std::ref(policies),
                                std::ref(partitions_[partition_idx]),
                                create_hyperslab,
                                object_id,
                                std::ref(property))
                                .get();

                            // write_partition(
                            //     policies, partitions_[partition_idx], create_hyperslab, object_id,
                            //     property);

                            partitions_.erase(partitions_.begin() + partition_idx);

                            partitions = std::move(partitions_);
                        }

#ifndef HDF5_IS_THREADSAFE
#else
#endif
                    });
        }

        template<typename Policies, typename Partitions>
        auto write_partitions_variable(
            Policies const& policies,
            hdf5::Offset const& array_hyperslab_start,  // Only needed to offset block written to array
            Partitions const& partitions,
            std::string const& array_pathname,
            data_model::ID const object_id,
            Index const time_step_idx) -> hpx::future<void>
        {
            auto const [dataset_pathname, phenomenon_name, property_set_name, property_name] =
                parse_array_pathname(array_pathname);

            // TODO: How to make all I/O happen on the I/O thread? Passing executor to continuation works in
            // parallel I/O case but serial case hangs. Currently, the nested I/O ends up on the compute
            // thread it seems.
            // hpx::execution::experimental::io_pool_executor executor;

            return hpx::when_any(partitions)
                .then(
                    // executor,
                    [  // executor,
                        policies,
                        array_hyperslab_start,
                        dataset_pathname,
                        phenomenon_name,
                        property_set_name,
                        property_name,
                        object_id,
                        time_step_idx](auto&& when_any_result_f)
                    {
                        AnnotateFunction const annotate{"to_lue: partitions variable"};

                        // Open the dataset once the first partition is ready to be written
                        auto dataset = open_dataset(dataset_pathname, H5F_ACC_RDWR);

                        // Now that dataset is open, write the ready partition to it

                        // Open phenomenon
                        auto& phenomenon{dataset.phenomena()[phenomenon_name]};

                        // Open property-set
                        auto& property_set{phenomenon.property_sets()[property_set_name]};

                        // Open property
                        lue_hpx_assert(property_set.properties().contains(property_name));
                        lue_hpx_assert(
                            property_set.properties().shape_per_object(property_name) ==
                            data_model::ShapePerObject::different);
                        lue_hpx_assert(
                            property_set.properties().value_variability(property_name) ==
                            data_model::ValueVariability::variable);
                        lue_hpx_assert(
                            property_set.properties().shape_variability(property_name) ==
                            data_model::ShapeVariability::constant);
                        using Properties = data_model::different_shape::constant_shape::Properties;
                        auto const& property{
                            property_set.properties().collection<Properties>()[property_name]};

                        using Partition = typename Partitions::value_type;
                        using PartitionServer = Partition::Server;

                        auto create_hyperslab =
                            [array_hyperslab_start, time_step_idx](PartitionServer const& partition_server)
                        { return hyperslab(array_hyperslab_start, partition_server, 0, time_step_idx); };

                        auto [partition_idx, partitions] = when_any_result_f.get();

                        auto write_partition = detail::write_partition<
                            std::remove_reference_t<std::remove_cv_t<Policies>>,
                            std::remove_reference_t<std::remove_cv_t<Partition>>,
                            std::remove_reference_t<std::remove_cv_t<decltype((create_hyperslab))>>,
                            std::remove_reference_t<std::remove_cv_t<decltype((property))>>>;

                        hpx::async(
                            // executor,
                            write_partition,
                            std::ref(policies),
                            std::ref(partitions[partition_idx]),
                            create_hyperslab,
                            object_id,
                            std::ref(property))
                            .get();

                        partitions.erase(partitions.begin() + partition_idx);

                        // Write any other ready partition, until all partition are written, one after the
                        // other
                        while (!partitions.empty())
                        {
                            auto [partition_idx, partitions_] = hpx::when_any(partitions).get();

                            hpx::async(
                                // executor,
                                write_partition,
                                std::ref(policies),
                                std::ref(partitions_[partition_idx]),
                                create_hyperslab,
                                object_id,
                                std::ref(property))
                                .get();

                            partitions_.erase(partitions_.begin() + partition_idx);

                            partitions = std::move(partitions_);
                        }
                    });
        }


        template<typename Policies, typename Partitions>
        struct WritePartitionsConstantAction:
            hpx::actions::make_action<
                decltype(&write_partitions_constant<Policies, Partitions>),
                &write_partitions_constant<Policies, Partitions>,
                WritePartitionsConstantAction<Policies, Partitions>>::type
        {
        };


        template<typename Policies, typename Partitions>
        struct WritePartitionsVariableAction:
            hpx::actions::make_action<
                decltype(&write_partitions_variable<Policies, Partitions>),
                &write_partitions_variable<Policies, Partitions>,
                WritePartitionsVariableAction<Policies, Partitions>>::type
        {
        };

    }  // namespace detail


    namespace policy::to_lue {

        template<typename InputElement>
        using DefaultPolicies =
            policy::DefaultPolicies<AllValuesWithinDomain<>, OutputElements<>, InputElements<InputElement>>;

        template<typename InputElement>
        using DefaultValuePolicies = policy::
            DefaultValuePolicies<AllValuesWithinDomain<>, OutputElements<>, InputElements<InputElement>>;

    }  // namespace policy::to_lue


    /*!
        @brief      Write an array to an array in a LUE dataset
        @tparam     Policies Policies type
        @tparam     Rank Rank of the array
        @param      policies Policies to use
        @param      array Array to write
        @param      array_pathname Pathname of the property to write to, formatted as
                    `<dataset_pathname>/<phenomenon_name>/<property_set_name>/<property_name>`
        @param      object_id ID of object whose property value to write
        @return     A future which becomes ready once the writing is done
    */
    template<typename Policies, Rank rank>
    [[nodiscard]] auto to_lue(
        Policies const& policies,
        PartitionedArray<policy::InputElementT<Policies>, rank> const& array,
        std::string const& array_pathname,
        data_model::ID const object_id) -> hpx::future<void>
    {
        AnnotateFunction const annotate{"to_lue"};

        using Element = policy::InputElementT<Policies>;
        using Array = PartitionedArray<Element, rank>;
        using Partition = PartitionT<Array>;

        auto const partition_idxs_by_locality{detail::partition_idxs_by_locality(array)};
        lue_hpx_assert(partition_idxs_by_locality.size() <= hpx::find_all_localities().size());

        std::vector<hpx::future<void>> localities_finished{};
        localities_finished.reserve(partition_idxs_by_locality.size() + 1);

#ifndef LUE_FRAMEWORK_WITH_PARALLEL_IO
        localities_finished.emplace_back(hpx::make_ready_future());
#endif

        using Action = detail::WritePartitionsConstantAction<Policies, std::vector<Partition>>;
        Action action{};

        for (auto const& [locality, partition_idxs] : partition_idxs_by_locality)
        {
            // Copy current partitions from input array to a new collection
            std::vector<Partition> partitions{};
            partitions.resize(partition_idxs.size());

            for (std::size_t idx = 0; auto const partition_idx : partition_idxs)
            {
                partitions[idx++] = array.partitions()[partition_idx];
            }

            // Spawn a task that writes the current partitions to the dataset. This returns a future which
            // becomes ready once the partitions have been written.
#ifndef LUE_FRAMEWORK_WITH_PARALLEL_IO
            hpx::future<void>& previous_locality_finished = localities_finished.back();
            localities_finished.push_back(previous_locality_finished.then(
                [locality,
                 action,
                 policies,
                 array_hyperslab = detail::shape_to_hyperslab(array.shape()),
                 partitions = std::move(partitions),
                 array_pathname,
                 object_id]([[maybe_unused]] auto const& previous_locality_finished)
                {
                    return hpx::async(
                        action,
                        locality,
                        std::move(policies),
                        array_hyperslab.start(),
                        std::move(partitions),
                        std::move(array_pathname),
                        object_id);
                }));
#else
            localities_finished.push_back(
                hpx::async(
                    action,
                    locality,
                    std::move(policies),
                    detail::shape_to_hyperslab(array.shape()).start(),
                    std::move(partitions),
                    std::move(array_pathname),
                    object_id));
#endif
        }

#ifndef LUE_FRAMEWORK_WITH_PARALLEL_IO
        lue_hpx_assert(localities_finished.back().valid());
        return std::move(localities_finished.back());
#else
        return hpx::when_all(localities_finished.begin(), localities_finished.end());
#endif
    }


    /*!
        @overload

        Default policies will be used.
    */
    template<typename Element, Rank rank>
    [[nodiscard]] auto to_lue(
        PartitionedArray<Element, rank> const& array,
        std::string const& array_pathname,
        data_model::ID const object_id) -> hpx::future<void>
    {
        using Policies = policy::to_lue::DefaultPolicies<Element>;

        return to_lue(Policies{}, array, array_pathname, object_id);
    }


    /*!
        @brief      Write an array to an array in a LUE dataset
        @tparam     Policies Policies type
        @tparam     Rank Rank of the array
        @param      policies Policies to use
        @param      array Array to write
        @param      array_pathname Pathname of the property to write to, formatted as
                    `<dataset_pathname>/<phenomenon_name>/<property_set_name>/<property_name>`
        @param      object_id ID of object whose property value to write
        @param      time_step_idx Index of time step to write
        @return     A future which becomes ready once the writing is done
    */
    template<typename Policies, Rank rank>
    [[nodiscard]] auto to_lue(
        Policies const& policies,
        PartitionedArray<policy::InputElementT<Policies>, rank> const& array,
        std::string const& array_pathname,
        data_model::ID const object_id,
        Index const time_step_idx) -> hpx::future<void>
    {
        AnnotateFunction const annotate{"to_lue"};

        using Element = policy::InputElementT<Policies>;
        using Array = PartitionedArray<Element, rank>;
        using Partition = PartitionT<Array>;

        // Group partitions by locality
        auto const partition_idxs_by_locality{detail::partition_idxs_by_locality(array)};
        lue_hpx_assert(partition_idxs_by_locality.size() <= hpx::find_all_localities().size());

        std::vector<hpx::future<void>> localities_finished{};
        localities_finished.reserve(partition_idxs_by_locality.size() + 1);

#ifndef LUE_FRAMEWORK_WITH_PARALLEL_IO
        localities_finished.emplace_back(hpx::make_ready_future());
#endif

        using Action = detail::WritePartitionsVariableAction<Policies, std::vector<Partition>>;
        Action action{};

        for (auto const& [locality, partition_idxs] : partition_idxs_by_locality)
        {
            // Copy current partitions from input array to a new collection
            std::vector<Partition> partitions{};
            partitions.resize(partition_idxs.size());

            for (std::size_t idx = 0; auto const partition_idx : partition_idxs)
            {
                partitions[idx++] = array.partitions()[partition_idx];
            }

            // Spawn a task that writes the current partitions to the dataset. This returns a future which
            // becomes ready once the partitions have been written.
#ifndef LUE_FRAMEWORK_WITH_PARALLEL_IO
            hpx::future<void>& previous_locality_finished = localities_finished.back();
            localities_finished.push_back(previous_locality_finished.then(
                [locality,
                 action,
                 policies,
                 array_hyperslab = detail::shape_to_hyperslab(array.shape()),
                 partitions = std::move(partitions),
                 array_pathname,
                 object_id,
                 time_step_idx]([[maybe_unused]] auto const& previous_locality_finished)
                {
                    return hpx::async(
                        action,
                        locality,
                        std::move(policies),
                        array_hyperslab.start(),
                        std::move(partitions),
                        std::move(array_pathname),
                        object_id,
                        time_step_idx);
                }));
#else
            localities_finished.push_back(
                hpx::async(
                    action,
                    locality,
                    std::move(policies),
                    detail::shape_to_hyperslab(array.shape()).start(),
                    std::move(partitions),
                    std::move(array_pathname),
                    object_id,
                    time_step_idx));
#endif
        }

#ifndef LUE_FRAMEWORK_WITH_PARALLEL_IO
        lue_hpx_assert(localities_finished.back().valid());
        return std::move(localities_finished.back());
#else
        return hpx::when_all(localities_finished.begin(), localities_finished.end());
#endif
    }


    /*!
        @overload

        Default policies will be used.
    */
    template<typename Element, Rank rank>
    [[nodiscard]] auto to_lue(
        PartitionedArray<Element, rank> const& array,
        std::string const& array_pathname,
        data_model::ID const object_id,
        Index const time_step_idx) -> hpx::future<void>
    {
        using Policies = policy::to_lue::DefaultPolicies<Element>;

        return to_lue(Policies{}, array, array_pathname, object_id, time_step_idx);
    }

}  // namespace lue
