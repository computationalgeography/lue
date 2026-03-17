#pragma once
#include "lue/framework/algorithm/create_partitioned_array.hpp"
#include "lue/framework/algorithm/definition/FastNoiseLite.h"
#include "lue/framework/algorithm/miscellaneous_operation_export.hpp"
#include "lue/framework/algorithm/open_simplex_noise.hpp"
#include "lue/framework/core/annotate.hpp"
#include "lue/framework/core/component.hpp"
#include "lue/macro.hpp"


namespace lue {
    namespace detail::open_simplex_noise {

        template<typename Policies, typename Partition>
        auto open_simplex_noise_partition(
            Policies const& policies,
            Partition const& x_coordinate_partition,
            Partition const& y_coordinate_partition,
            int const seed) -> PartitionT<Partition, policy::OutputElementT<Policies, 0>>
        {
            using Offset = OffsetT<Partition>;
            using Shape = ShapeT<Partition>;
            using Data = DataT<Partition>;

            static_assert(rank<Shape> == 2);

            lue_hpx_assert(x_coordinate_partition.is_ready());
            lue_hpx_assert(y_coordinate_partition.is_ready());

            return hpx::dataflow(
                hpx::launch::async,
                hpx::unwrapping(

                    [policies, x_coordinate_partition, y_coordinate_partition, seed](
                        Offset const& offset, Data const& x_coordinates, Data const& y_coordinates) -> auto
                    {
                        AnnotateFunction annotation{"open_simplex_noise: partition"};

                        HPX_UNUSED(x_coordinate_partition);
                        HPX_UNUSED(y_coordinate_partition);

                        Data output_partition_data{x_coordinates.shape()};

                        // Iterate over all cells and call the noise function, passing in the coordinates

                        // Just the noise and nothing else
                        FastNoiseLite noise{seed};
                        noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);

                        auto const& dp = policies.domain_policy();
                        auto const& indp1 = std::get<0>(policies.inputs_policies()).input_no_data_policy();
                        auto const& indp2 = std::get<1>(policies.inputs_policies()).input_no_data_policy();
                        auto const& ondp = std::get<0>(policies.outputs_policies()).output_no_data_policy();
                        auto const& rp = std::get<0>(policies.outputs_policies()).range_policy();

                        Count const nr_elements{lue::nr_elements(x_coordinates)};

                        for (Index cell_idx = 0; cell_idx < nr_elements; ++cell_idx)
                        {
                            if (indp1.is_no_data(x_coordinates, cell_idx) ||
                                indp2.is_no_data(y_coordinates, cell_idx))
                            {
                                ondp.mark_no_data(output_partition_data, cell_idx);
                            }
                            else if (!dp.within_domain(x_coordinates[cell_idx], y_coordinates[cell_idx]))
                            {
                                ondp.mark_no_data(output_partition_data, cell_idx);
                            }
                            else
                            {
                                output_partition_data[cell_idx] =
                                    noise.GetNoise(x_coordinates[cell_idx], y_coordinates[cell_idx]);

                                if (!rp.within_range(
                                        x_coordinates[cell_idx],
                                        y_coordinates[cell_idx],
                                        output_partition_data[cell_idx]))
                                {
                                    ondp.mark_no_data(output_partition_data, cell_idx);
                                }
                            }
                        }

                        return Partition{hpx::find_here(), offset, std::move(output_partition_data)};
                    }

                    ),
                x_coordinate_partition.offset(),
                x_coordinate_partition.data(),
                y_coordinate_partition.data());
        }


        template<typename Policies, typename Partition>
        struct OpenSimplex2PartitionAction:
            hpx::actions::make_action<
                decltype(&open_simplex_noise_partition<Policies, Partition>),
                &open_simplex_noise_partition<Policies, Partition>,
                OpenSimplex2PartitionAction<Policies, Partition>>::type
        {
        };

    }  // namespace detail::open_simplex_noise


    template<typename Policies, Rank rank>
        requires(
            std::is_floating_point_v<policy::OutputElementT<Policies, 0>> &&
            std::is_same_v<policy::OutputElementT<Policies, 0>, policy::InputElementT<Policies, 0>> &&
            std::is_same_v<policy::OutputElementT<Policies, 0>, policy::InputElementT<Policies, 1>>)
    auto open_simplex_noise(
        Policies const& policies,
        PartitionedArray<policy::InputElementT<Policies, 0>, rank> const& x_coordinates,
        PartitionedArray<policy::InputElementT<Policies, 1>, rank> const& y_coordinates,
        int const seed) -> PartitionedArray<policy::OutputElementT<Policies, 0>, rank>
    {
        using Element = policy::OutputElementT<Policies, 0>;
        using Array = PartitionedArray<Element, rank>;
        using Partitions = PartitionsT<Array>;
        using Partition = PartitionT<Array>;

        lue_hpx_assert(all_are_valid(x_coordinates.partitions()));
        lue_hpx_assert(all_are_valid(y_coordinates.partitions()));

        detail::open_simplex_noise::OpenSimplex2PartitionAction<Policies, Partition> action;

        Localities<rank> localities{x_coordinates.localities()};
        Partitions const& x_coordinate_partitions{x_coordinates.partitions()};
        Partitions const& y_coordinate_partitions{y_coordinates.partitions()};
        Partitions output_partitions{shape_in_partitions(x_coordinates)};

        for (Index partition_idx = 0; partition_idx < nr_partitions(x_coordinates); ++partition_idx)
        {
            output_partitions[partition_idx] = hpx::dataflow(
                hpx::launch::async,

                [locality_id = localities[partition_idx], action, policies, seed](
                    Partition const& x_coordinate_partition, Partition const& y_coordinate_partition) -> auto
                {
                    return action(
                        locality_id, policies, x_coordinate_partition, y_coordinate_partition, seed);
                },

                x_coordinate_partitions[partition_idx],
                y_coordinate_partitions[partition_idx]);
        }

        return Array{shape(x_coordinates), std::move(localities), std::move(output_partitions)};
    }

}  // namespace lue


#define LUE_INSTANTIATE_OPEN_SIMPLEX_NOISE(Policies, rank)                                                   \
                                                                                                             \
    template LUE_MISCELLANEOUS_OPERATION_EXPORT PartitionedArray<policy::OutputElementT<Policies, 0>, rank>  \
    open_simplex_noise<ArgumentType<void(Policies)>, rank>(                                                  \
        ArgumentType<void(Policies)> const&,                                                                 \
        PartitionedArray<policy::InputElementT<Policies, 0>, rank> const&,                                   \
        PartitionedArray<policy::InputElementT<Policies, 1>, rank> const&,                                   \
        int);
