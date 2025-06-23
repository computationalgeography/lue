#pragma once
#include "lue/framework/algorithm/copy.hpp"
#include "lue/framework/algorithm/create_partitioned_array.hpp"
#include "lue/framework/algorithm/detail/resample.hpp"
#include "lue/framework/algorithm/miscellaneous_operation_export.hpp"
#include "lue/framework/algorithm/resample.hpp"
#include "lue/framework/core/annotate.hpp"
#include "lue/macro.hpp"


namespace lue {
    namespace detail::downscale {

        template<typename Policies, Rank rank>
        auto assign_partition(
            Policies const& policies,
            PartitionT<PartitionedArray<policy::InputElementT<Policies, 0>, rank>> const& source_partition,
            Offset<Index, rank> const& source_area_offset,
            Count const count,
            Offset<Index, rank> const& target_partition_offset,
            Shape<Count, rank> const& target_partition_shape)
            -> PartitionT<PartitionedArray<policy::OutputElementT<Policies, 0>, rank>>

        {
            // NOTE: There is no guarantee that the source partition is located in the current locality!

            // - Wait for the source partition to be ready
            // - Create the target partition
            // - Copy values from the source partition to the target partition

            using Element = policy::InputElementT<Policies, 0>;
            using Partition = PartitionT<PartitionedArray<Element, rank>>;
            using Data = DataT<Partition>;

            return hpx::dataflow(
                hpx::launch::async,
                [policies, source_area_offset, count, target_partition_offset, target_partition_shape](
                    Partition const& source_partition) -> Partition
                {
                    return hpx::dataflow(
                        hpx::launch::async,
                        hpx::unwrapping(
                            [source_partition,
                             policies,
                             source_area_offset,
                             count,
                             target_partition_offset,
                             target_partition_shape](Data const& source_partition_data) -> Partition
                            {
                                AnnotateFunction const annotation{"downscale_assign: partition"};

                                HPX_UNUSED(source_partition);

                                Data target_partition_data{target_partition_shape, 1};

                                auto const& indp =
                                    std::get<0>(policies.inputs_policies()).input_no_data_policy();
                                auto const& ondp =
                                    std::get<0>(policies.outputs_policies()).output_no_data_policy();

                                // Determine whether / how many of the source cells in the previous row and
                                // column have already been assigned to target partition cells of neighbouring
                                // partitions. The task only needs to copy the remaining number of source
                                // cells.
                                auto const nr_target_cells_remaining0 =
                                    std::get<0>(target_partition_offset) % count;
                                auto const nr_target_cells_remaining1 =
                                    std::get<1>(target_partition_offset) % count;

                                auto const [nr_target_elements0, nr_target_elements1] =
                                    target_partition_shape;

                                // std::cout << std::format(
                                //     "-------------------------------------------------------------------\n"
                                //     "- source_area_offset       : ({}, {})\n"
                                //     "- source_partition_shape   : ({}, {})\n"
                                //     "- target_partition_offset  : ({}, {})\n"
                                //     "- target_partition_shape   : ({}, {})\n"
                                //     "- count                    : {}\n"
                                //     "- nr_target_cells_remaining: ({}, {})\n",
                                //     source_idx0,
                                //     source_idx1,
                                //     nr_source_elements0,
                                //     nr_source_elements1,
                                //     std::get<0>(target_partition_offset),
                                //     std::get<1>(target_partition_offset),
                                //     nr_target_elements0,
                                //     nr_target_elements1,
                                //     count,
                                //     nr_target_cells_remaining0,
                                //     nr_target_cells_remaining1);

#ifndef NDEBUG
                                auto const [nr_source_elements0, nr_source_elements1] =
                                    source_partition_data.shape();

                                // Upper left cell of area to copy from
                                auto [source_idx0, source_idx1] = source_area_offset;

                                lue_hpx_assert(
                                    count * (nr_source_elements0 - source_idx0) >= nr_target_elements0);
                                lue_hpx_assert(
                                    count * (nr_source_elements1 - source_idx1) >= nr_target_elements1);
#endif

                                // Iterate over source cells to copy until target partition is filled
                                Count nr_copies_to_make0 = count - nr_target_cells_remaining0;

                                for (Index source_idx0 = std::get<0>(source_area_offset), target_idx0 = 0;
                                     target_idx0 < nr_target_elements0;
                                     ++source_idx0)
                                {
                                    nr_copies_to_make0 =
                                        std::min(nr_copies_to_make0, nr_target_elements0 - target_idx0);
                                    Count nr_copies_to_make1 = count - nr_target_cells_remaining1;

                                    for (Index source_idx1 = std::get<1>(source_area_offset), target_idx1 = 0;
                                         target_idx1 < nr_target_elements1;
                                         ++source_idx1)
                                    {
                                        nr_copies_to_make1 =
                                            std::min(nr_copies_to_make1, nr_target_elements1 - target_idx1);

                                        // Copy source cell's value and, assign it to target cells

                                        if (indp.is_no_data(source_partition_data, source_idx0, source_idx1))
                                        {
                                            for (Index idx0 = 0; idx0 < nr_copies_to_make0; ++idx0)
                                            {
                                                for (Index idx1 = 0; idx1 < nr_copies_to_make1; ++idx1)
                                                {
                                                    ondp.mark_no_data(
                                                        target_partition_data,
                                                        target_idx0 + idx0,
                                                        target_idx1 + idx1);
                                                }
                                            }
                                        }
                                        else
                                        {
                                            for (Index idx0 = 0; idx0 < nr_copies_to_make0; ++idx0)
                                            {
                                                for (Index idx1 = 0; idx1 < nr_copies_to_make1; ++idx1)
                                                {
                                                    target_partition_data(
                                                        target_idx0 + idx0, target_idx1 + idx1) =
                                                        source_partition_data(source_idx0, source_idx1);
                                                }
                                            }
                                        }

                                        target_idx1 += nr_copies_to_make1;
                                        nr_copies_to_make1 = count;
                                    }

                                    target_idx0 += nr_copies_to_make0;
                                    nr_copies_to_make0 = count;
                                }

                                return Partition{
                                    hpx::find_here(),
                                    target_partition_offset,
                                    std::move(target_partition_data)};
                            }),
                        source_partition.data());
                },
                source_partition);
        }


        template<typename Policies, Rank rank>
        struct AssignPartitionAction:
            hpx::actions::make_action<
                decltype(&assign_partition<Policies, rank>),
                &assign_partition<Policies, rank>,
                AssignPartitionAction<Policies, rank>>::type
        {
        };


        template<typename Element, lue::Rank rank>
        class Assign
        {
            public:

                using OutputElement = Element;
                using Partition = lue::ArrayPartition<OutputElement, rank>;
                using Offset = lue::OffsetT<Partition>;
                using Shape = lue::ShapeT<Partition>;

                static constexpr bool instantiate_per_locality{false};


                Assign(
                    PartitionedArray<Element, rank>::Partitions const& partitions,
                    Shape const& partition_shape,
                    Count const count):

                    _partitions{partitions},
                    _partition_shape{partition_shape},
                    _count{count}

                {
                    lue_hpx_assert(count >= 1);
                }


                template<typename Policies>
                auto instantiate(
                    hpx::id_type const locality_id,
                    [[maybe_unused]] Policies const& policies,
                    [[maybe_unused]] Shape const& target_array_shape,
                    Shape const& target_shape_in_partitions,
                    [[maybe_unused]] lue::Index const target_partition_idx_along_curve,
                    lue::Index const target_partition_idx,
                    Offset const& target_partition_offset,
                    Shape const& target_partition_shape) const -> Partition
                {
                    // Spawn a task that will execute on the target locality and does whatever it takes to
                    // downscale from a source partition to a target partition

                    // Given that the partition shape of the source and target array is the same (except maybe
                    // for the partitions in the last row and column), only a single source partition is
                    // needed to fill a single target partition.

                    // The partition_idx passed in is the linear index of the partition to create. Determine
                    // the partition idx of the first source partition contributing values to the target
                    // partition.
                    auto const source_partition_idx = target_idx_to_source_idx(
                        target_shape_in_partitions, target_partition_idx, _partitions.shape(), _count);
                    lue_hpx_assert(source_partition_idx < _partitions.nr_elements());

                    // The task needs to assign source cell values to target cell values. A single source cell
                    // contributes to the values of multiple target cells. Determine the offset and shape of
                    // the source cells that must be assigned to the cells in the target partition.

                    Offset source_area_offset{
                        (std::get<0>(target_partition_offset) / _count) % std::get<0>(_partition_shape),
                        (std::get<1>(target_partition_offset) / _count) % std::get<1>(_partition_shape)};

                    // Careful: in case of the last row and column, partition shapes may differ. Only in those
                    // cases, we need to test the special situation that we need to query outside of the
                    // normal partition bounds. For example, if the partition shape is (3, 3), then a last
                    // partition in a row may have shape (3, 5). Without doing the following test, we would
                    // copy the wrong source values (starting at offset (0, 0), while instead we should be
                    // copying those starting at (0, 3).
                    if (std::get<0>(target_partition_shape) != std::get<0>(_partition_shape) &&
                        std::get<0>(source_area_offset) == 0)
                    {
                        std::get<0>(source_area_offset) = std::get<0>(_partition_shape);
                    }

                    if (std::get<1>(target_partition_shape) != std::get<1>(_partition_shape) &&
                        std::get<1>(source_area_offset) == 0)
                    {
                        std::get<1>(source_area_offset) = std::get<1>(_partition_shape);
                    }

                    // To only thing the task needs to do is copy cells from the source area, starting at the
                    // offset, into the new partition, assigning source cell values to multiple target
                    // cells.

                    AssignPartitionAction<Policies, rank> action{};

                    return hpx::async(
                        action,
                        locality_id,
                        policies,
                        _partitions[source_partition_idx],
                        source_area_offset,
                        _count,
                        target_partition_offset,
                        target_partition_shape);
                }

            private:

                //! Collection of input partitions to downscale
                PartitionedArray<Element, rank>::Partitions _partitions;

                //! Partition shape of the first partition
                Shape _partition_shape;

                //! Number of cells to split each cell in, along each dimension
                Count _count;
        };


        template<typename Policies, Rank rank>
        auto downscale_assign(
            Policies const& policies,
            PartitionedArray<policy::InputElementT<Policies, 0>, rank> const& array,
            Count const count) -> PartitionedArray<policy::OutputElementT<Policies, 0>, rank>
        {
            static_assert(
                std::is_same_v<policy::InputElementT<Policies, 0>, policy::OutputElementT<Policies, 0>>);
            static_assert(rank == 2);

            using Element = policy::InputElementT<Policies, 0>;
            using Array = PartitionedArray<Element, rank>;
            using Shape = ShapeT<Array>;

            if (count == Count{0})
            {
                throw std::runtime_error("Downscaling requireѕ that count is ≥ one, but zero was passed in");
            }

            if (count == Count{1})
            {
                // Silly. Just copy the input array.
                return copy(policies, array);
            }

            Shape const source_array_shape{array.shape()};
            Shape const target_array_shape{
                count * std::get<0>(source_array_shape), count * std::get<1>(source_array_shape)};

            using Functor = Assign<Element, rank>;

            // Creating a partitioned array requires a partition shape. We use the same partition shape as
            // the one used by the source array. This "blocks" on the first partition.
            array.partitions()[0].wait();
            auto const partition_shape = array.partitions()[0].shape().get();

            return create_partitioned_array(
                policies,
                target_array_shape,
                partition_shape,
                Functor{array.partitions(), partition_shape, count});
        }

    }  // namespace detail::downscale


    template<typename Policies, Rank rank>
    auto downscale(
        Policies const& policies,
        PartitionedArray<policy::InputElementT<Policies, 0>, rank> const& array,
        Count const count,
        DownscaleStrategy const strategy) -> PartitionedArray<policy::OutputElementT<Policies, 0>, rank>
    {
        static_assert(
            std::is_same_v<policy::InputElementT<Policies, 0>, policy::OutputElementT<Policies, 0>>);
        static_assert(rank == 2);

        AnnotateFunction const annotation{"downscale: array"};

        switch (strategy)
        {
            // Strategies that don't use a minimum contributing threshold
            case DownscaleStrategy::assign:
            {
                return detail::downscale::downscale_assign(policies, array, count);
                break;
            }
            default:
            {
                throw std::runtime_error(
                    "Unsupported downscaling strategy passed. Use one of: assign, or call a different "
                    "overload");
            }
        }
    }

}  // namespace lue


#define LUE_INSTANTIATE_DOWNSCALE(Policies, rank)                                                            \
                                                                                                             \
    template LUE_MISCELLANEOUS_OPERATION_EXPORT PartitionedArray<policy::OutputElementT<Policies, 0>, rank>  \
    downscale<ArgumentType<void(Policies)>, rank>(                                                           \
        ArgumentType<void(Policies)> const&,                                                                 \
        PartitionedArray<policy::InputElementT<Policies, 0>, rank> const&,                                   \
        Count,                                                                                               \
        DownscaleStrategy);
