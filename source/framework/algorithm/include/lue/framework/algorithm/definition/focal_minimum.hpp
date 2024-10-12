#pragma once
#include "lue/framework/algorithm/definition/focal_operation.hpp"
#include "lue/framework/algorithm/focal_minimum.hpp"
#include "lue/framework/algorithm/focal_operation_export.hpp"


namespace lue {
    namespace detail {

        template<typename InputElement>
        class FocalMinimum
        {

            public:

                static constexpr char const* name{"focal_minimum"};

                using OutputElement = InputElement;


                template<typename Kernel, typename OutputPolicies, typename InputPolicies, typename Subspan>
                OutputElement operator()(
                    Kernel const& kernel,
                    OutputPolicies const& output_policies,
                    InputPolicies const& input_policies,
                    Subspan const& window) const
                {
                    static_assert(rank<Kernel> == 2);

                    using Weight = ElementT<Kernel>;

                    static_assert(std::is_integral_v<Weight>);

                    lue_hpx_assert(window.extent(0) == kernel.size());
                    lue_hpx_assert(window.extent(1) == kernel.size());

                    auto const& indp = input_policies.input_no_data_policy();
                    auto const& ondp = output_policies.output_no_data_policy();

                    OutputElement min{};
                    bool initialized{false};

                    for (Index r = 0; r < window.extent(0); ++r)
                    {
                        for (Index c = 0; c < window.extent(1); ++c)
                        {
                            Weight const weight{kernel(r, c)};
                            InputElement const value{window(r, c)};

                            if (indp.is_no_data(value))
                            {
                                // In case one of the cells within the window contains a no-data
                                // value, the result is marked as no-data
                                initialized = false;
                                r = window.extent(0);
                                c = window.extent(1);
                            }
                            else
                            {
                                if (weight)
                                {
                                    if (!initialized)
                                    {
                                        min = value;
                                        initialized = true;
                                    }
                                    else
                                    {
                                        min = std::min(min, value);
                                    }
                                }
                            }
                        }
                    }

                    if (!initialized)
                    {
                        ondp.mark_no_data(min);
                    }

                    return min;
                }
        };

    }  // namespace detail


    template<typename Policies, typename Element, Rank rank, typename Kernel>
    PartitionedArray<Element, rank> focal_minimum(
        Policies const& policies, PartitionedArray<Element, rank> const& array, Kernel const& kernel)
    {
        using Functor = detail::FocalMinimum<Element>;

        return focal_operation(policies, array, kernel, Functor{});
    }

}  // namespace lue


#define LUE_INSTANTIATE_FOCAL_MINIMUM(Policies, Element, Kernel)                                             \
                                                                                                             \
    template LUE_FOCAL_OPERATION_EXPORT PartitionedArray<Element, 2>                                         \
    focal_minimum<ArgumentType<void(Policies)>, Element, 2, Kernel>(                                         \
        ArgumentType<void(Policies)> const&, PartitionedArray<Element, 2> const&, Kernel const&);
