#pragma once
#include "lue/framework/algorithm/aspect.hpp"
#include "lue/framework/algorithm/focal_operation_export.hpp"
#include "lue/framework/algorithm/definition/focal_operation.hpp"
#include "lue/framework/algorithm/serialize/kernel.hpp"
#include "lue/macro.hpp"
#include <boost/math/constants/constants.hpp>


namespace lue {
    namespace detail {

        template<
            typename T>
        T radians_to_compass_direction(
            T angle)
        {
#ifndef NDEBUG
            static T const pi = boost::math::constants::pi<T>();
#endif
            static T const half_pi = boost::math::constants::half_pi<T>();
            static T const two_pi = boost::math::constants::two_pi<T>();

            assert(angle >= -pi && angle <= pi);

            // Input angle, in radians:
            // east :  0.0 pi
            // north:  0.5 pi
            // west :  1.0 pi
            // south: -0.5 pi

            // Output angle, in radians:
            // north: 0.0 pi
            // east : 0.5 pi
            // south: 1.0 pi
            // west : 1.5 pi

            if(angle < T{0})
            {
                angle = half_pi - angle;
            }
            else if(angle > half_pi)
            {
                angle = two_pi - angle + half_pi;
            }
            else
            {
                angle = half_pi - angle;
            }

            assert(angle >= T{0} && angle < T{2} * pi);

            return angle;
        }


        template<
            typename Element>
        class Aspect
        {

            public:

                using OutputElement = Element;


                template<
                    typename Kernel,
                    typename OutputPolicies,
                    typename InputPolicies,
                    typename Subspan>
                OutputElement operator()(
                    [[maybe_unused]] Kernel const& kernel,
                    OutputPolicies const& output_policies,
                    InputPolicies const& input_policies,
                    Subspan const& elevation_window) const
                {
                    static_assert(rank<Kernel> == 2);
                    lue_hpx_assert(kernel.radius() == 1);

                    auto const& indp{input_policies.input_no_data_policy()};
                    auto const& ondp{output_policies.output_no_data_policy()};

                    Element aspect;

                    if(indp.is_no_data(elevation_window(1, 1)))
                    {
                        ondp.mark_no_data(aspect);
                    }
                    else
                    {
                        // TODO: all surrounding cells must have a valid value!!!
                        //       Once done, no-data tests can be removed here. Temp variables
                        //       can be remove too.

                        // west - east
                        // dz_dx = (w[0, 0] + 2 * w[1, 0] + w[2, 0]) - (w[0, 2] + 2 * w[1, 2] + w[2, 2])
                        //       = tmp1 - tmp2

                        // south - north
                        // dz_dy = (w[2, 0] + 2 * w[2, 1] + w[2, 2]) - (w[0, 0] + 2 * w[0, 1] + w[0, 2])
                        //       = tmp3 - tmp4

                        Element tmp1{0};

                        if(!indp.is_no_data(elevation_window(0, 0)))
                        {
                            tmp1 += elevation_window(0, 0);
                        }

                        if(!indp.is_no_data(elevation_window(1, 0)))
                        {
                            tmp1 += 2 * elevation_window(1, 0);
                        }

                        if(!indp.is_no_data(elevation_window(2, 0)))
                        {
                            tmp1 += elevation_window(2, 0);
                        }

                        Element tmp2{0};

                        if(!indp.is_no_data(elevation_window(0, 2)))
                        {
                            tmp2 += elevation_window(0, 2);
                        }

                        if(!indp.is_no_data(elevation_window(1, 2)))
                        {
                            tmp2 += 2 * elevation_window(1, 2);
                        }

                        if(!indp.is_no_data(elevation_window(2, 2)))
                        {
                            tmp2 += elevation_window(2, 2);
                        }

                        Element tmp3{0};

                        if(!indp.is_no_data(elevation_window(2, 0)))
                        {
                            tmp3 += elevation_window(2, 0);
                        }

                        if(!indp.is_no_data(elevation_window(2, 1)))
                        {
                            tmp3 += 2 * elevation_window(2, 1);
                        }

                        if(!indp.is_no_data(elevation_window(2, 2)))
                        {
                            tmp3 += elevation_window(2, 2);
                        }

                        Element tmp4{0};

                        if(!indp.is_no_data(elevation_window(0, 0)))
                        {
                            tmp4 += elevation_window(0, 0);
                        }

                        if(!indp.is_no_data(elevation_window(0, 1)))
                        {
                            tmp4 += 2 * elevation_window(0, 1);
                        }

                        if(!indp.is_no_data(elevation_window(0, 2)))
                        {
                            tmp4 += elevation_window(0, 2);
                        }

                        Element const dz_dx = tmp1 - tmp2;
                        Element const dz_dy = tmp3 - tmp4;

                        aspect = (dz_dx == Element{0} && dz_dy == Element{0})
                            ? -1  // Flat cell, pointing upwards
                            : radians_to_compass_direction(std::atan2(dz_dy, dz_dx))
                            ;
                    }

                    return aspect;
                }

        };

    }  // namespace detail


    template<
        typename Policies,
        typename Element,
        Rank rank>
    PartitionedArray<Element, rank> aspect(
        Policies const& policies,
        PartitionedArray<Element, rank> const& elevation)
    {
        using Functor = detail::Aspect<Element>;

        // Only used for its radius. Weights are not used.
        auto kernel{box_kernel<bool, 2>(1, true)};

        return focal_operation(policies, elevation, std::move(kernel), Functor{});
    }

}  // namespace lue


#define LUE_INSTANTIATE_ASPECT(                         \
    Policies, Element)                                  \
                                                        \
    template LUE_FOCAL_OPERATION_EXPORT                 \
    PartitionedArray<Element, 2> aspect<                \
            ArgumentType<void(Policies)>, Element, 2>(  \
        ArgumentType<void(Policies)> const&,            \
        PartitionedArray<Element, 2> const&);
