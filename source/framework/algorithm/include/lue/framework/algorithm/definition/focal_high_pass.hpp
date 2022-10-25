#pragma once
#include "lue/framework/algorithm/focal_high_pass.hpp"
#include "lue/framework/algorithm/focal_operation_export.hpp"
#include "lue/framework/algorithm/definition/focal_operation.hpp"


namespace lue {
    namespace detail {

        template<
            typename InputElement>
        class FocalHighPass
        {

            public:

                // If not, an OutputElement type must be passed in
                static_assert(std::is_floating_point_v<InputElement>);

                using OutputElement = InputElement;


                template<
                    typename Kernel,
                    typename OutputPolicies,
                    typename InputPolicies,
                    typename Subspan>
                OutputElement operator()(
                    Kernel const& kernel,
                    OutputPolicies const& output_policies,
                    InputPolicies const& input_policies,
                    Subspan const& window) const
                {
                    static_assert(rank<Kernel> == 2);

                    using Weight = ElementT<Kernel>;

                    Radius const radius{kernel.radius()};

                    static_assert(std::is_integral_v<Weight>);

                    lue_hpx_assert(window.extent(0) == kernel.size());
                    lue_hpx_assert(window.extent(1) == kernel.size());

                    auto const& indp = input_policies.input_no_data_policy();
                    auto const& ondp = output_policies.output_no_data_policy();

                    OutputElement focal_value;
                    OutputElement sum_of_neighbours{0};
                    Count nr_neighbours{0};

                    for(Index r = 0; r < window.extent(0); ++r) {
                        for(Index c = 0; c < window.extent(1); ++c)
                        {
                            InputElement const value{window(r, c)};

                            if(r == radius && c == radius)
                            {
                                // Remember the focal value. Its kernel weight is irrelevant.
                                focal_value = value;
                            }
                            else
                            {
                                if(indp.is_no_data(value))
                                {
                                    // In case one of the cells within the window contains a no-data
                                    // value, the result is marked as no-data
                                    nr_neighbours = 0;
                                    r = window.extent(0);
                                    c = window.extent(1);
                                }
                                else
                                {
                                    Weight const weight{kernel(r, c)};

                                    if(weight)
                                    {
                                        sum_of_neighbours += value;
                                        ++nr_neighbours;
                                    }
                                }
                            }
                        }
                    }

                    OutputElement high_pass;

                    {
                        if(nr_neighbours == 0)
                        {
                            ondp.mark_no_data(high_pass);
                        }
                        else
                        {
                            high_pass = 2 * focal_value * nr_neighbours - sum_of_neighbours;
                        }
                    }

                    return high_pass;
                }

        };

    }  // namespace detail


    template<
        typename Policies,
        typename Element,
        Rank rank,
        typename Kernel>
    PartitionedArray<Element, rank> focal_high_pass(
        Policies const& policies,
        PartitionedArray<Element, rank> const& array,
        Kernel const& kernel)
    {
        using Functor = detail::FocalHighPass<Element>;

        return focal_operation(policies, array, kernel, Functor{});
    }

}  // namespace lue


#define LUE_INSTANTIATE_FOCAL_HIGH_PASS(                        \
    Policies, Element, Kernel)                                  \
                                                                \
    template LUE_FOCAL_OPERATION_EXPORT                         \
    PartitionedArray<Element, 2> focal_high_pass<               \
            ArgumentType<void(Policies)>, Element, 2, Kernel>(  \
        ArgumentType<void(Policies)> const&,                    \
        PartitionedArray<Element, 2> const&,                    \
        Kernel const&);
