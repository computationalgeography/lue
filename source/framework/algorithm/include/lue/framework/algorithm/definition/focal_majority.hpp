#pragma once
#include "lue/framework/algorithm/definition/focal_operation.hpp"
#include "lue/framework/algorithm/focal_majority.hpp"
#include "lue/framework/algorithm/focal_operation_export.hpp"
#include <unordered_map>


namespace lue {
    namespace detail {

        template<typename InputElement>
        class FocalMajority
        {

            public:

                static_assert(std::is_integral_v<InputElement>);

                static constexpr char const* name{"focal_majority"};

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

                    std::unordered_map<InputElement, Count> frequencies;

                    for (Index r = 0; r < window.extent(0); ++r)
                    {
                        for (Index c = 0; c < window.extent(1); ++c)
                        {
                            InputElement const value{window(r, c)};

                            if (indp.is_no_data(value))
                            {
                                // In case one of the cells within the window contains a no-data
                                // value, the result is marked as no-data
                                frequencies.clear();
                                r = window.extent(0);
                                c = window.extent(1);
                            }
                            else
                            {
                                Weight const weight{kernel(r, c)};

                                if (weight)
                                {
                                    ++frequencies[value];
                                }
                            }
                        }
                    }

                    OutputElement majority_value;

                    {
                        Count majority_value_frequency{0};
                        bool majority_value_is_unique{false};

                        if (frequencies.empty())
                        {
                            ondp.mark_no_data(majority_value);
                        }
                        else
                        {
                            // Find value with largest frequency. If there are multiple, output
                            // a no-data.

                            for (auto const& [value, frequency] : frequencies)
                            {
                                if (frequency > majority_value_frequency)
                                {
                                    // Potential result
                                    majority_value_frequency = frequency;
                                    majority_value = value;
                                    majority_value_is_unique = true;
                                }
                                else if (frequency == majority_value_frequency)
                                {
                                    // Multiple potential results
                                    majority_value_is_unique = false;
                                }
                            }

                            if (!majority_value_is_unique)
                            {
                                ondp.mark_no_data(majority_value);
                            }
                        }
                    }

                    return majority_value;
                }
        };

    }  // namespace detail


    template<typename Policies, typename Element, Rank rank, typename Kernel>
    PartitionedArray<Element, rank> focal_majority(
        Policies const& policies, PartitionedArray<Element, rank> const& array, Kernel const& kernel)
    {
        using Functor = detail::FocalMajority<Element>;

        return focal_operation(policies, array, kernel, Functor{});
    }

}  // namespace lue


#define LUE_INSTANTIATE_FOCAL_MAJORITY(Policies, Element, Kernel)                                            \
                                                                                                             \
    template LUE_FOCAL_OPERATION_EXPORT PartitionedArray<Element, 2>                                         \
    focal_majority<ArgumentType<void(Policies)>, Element, 2, Kernel>(                                        \
        ArgumentType<void(Policies)> const&, PartitionedArray<Element, 2> const&, Kernel const&);
