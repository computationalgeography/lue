#pragma once
#include "lue/framework/algorithm/definition/focal_operation.hpp"
#include "lue/framework/algorithm/focal_diversity.hpp"
#include "lue/framework/algorithm/focal_operation_export.hpp"
#include <algorithm>
#include <set>


namespace lue {
    namespace detail {

        template<typename Count, typename Element>
        class FocalDiversity
        {

            public:

                using OutputElement = Count;
                using InputElement = Element;

                static_assert(std::is_integral_v<InputElement>);
                static_assert(std::is_integral_v<OutputElement>);

                static constexpr char const* name{"focal_diversity"};


                template<typename Kernel, typename OutputPolicies, typename InputPolicies, typename Subspan>
                Count operator()(
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

                    // NOTE: For each cell, this will allocate memory on the heap, which is slow.
                    //     The caller is iterating over all cells within a partition. For these calls,
                    //     a single allocated buffer could be reused. But multiple callers might
                    //     be calling us at the same time, so we can't just use a static variable.
                    std::vector<InputElement> values{};
                    values.reserve(nr_elements(kernel));

                    for (Index r = 0; r < window.extent(0); ++r)
                    {
                        for (Index c = 0; c < window.extent(1); ++c)
                        {
                            Weight const weight{kernel(r, c)};
                            InputElement const value{window[r, c]};

                            if (indp.is_no_data(value))
                            {
                                // In case one of the cells within the window contains a no-data
                                // value, the result is marked as no-data
                                values.clear();
                                r = window.extent(0);
                                c = window.extent(1);
                            }
                            else
                            {
                                if (weight)
                                {
                                    values.push_back(value);
                                }
                            }
                        }
                    }

                    Count count;

                    {
                        if (values.empty())
                        {
                            // No valid values found
                            ondp.mark_no_data(count);
                        }
                        else
                        {
                            std::sort(values.begin(), values.end());
                            count = static_cast<Count>(
                                std::unique(values.begin(), values.end()) - values.begin());
                        }
                    }

                    return count;
                }
        };

    }  // namespace detail


    /*!
      @brief      Return an array with per cell the number of unique element values found in the cells within
                  the corresponding focal window
      @ingroup    focal_operation
    */
    template<typename Count, typename Policies, typename Element, Rank rank, typename Kernel>
    PartitionedArray<Count, rank> focal_diversity(
        Policies const& policies, PartitionedArray<Element, rank> const& array, Kernel const& kernel)
    {
        using Functor = detail::FocalDiversity<Count, Element>;

        return focal_operation(policies, array, kernel, Functor{});
    }

}  // namespace lue


#define LUE_INSTANTIATE_FOCAL_DIVERSITY(Policies, Count, Element, Kernel)                                    \
                                                                                                             \
    template LUE_FOCAL_OPERATION_EXPORT PartitionedArray<Count, 2>                                           \
    focal_diversity<Count, ArgumentType<void(Policies)>, Element, 2, Kernel>(                                \
        ArgumentType<void(Policies)> const&, PartitionedArray<Element, 2> const&, Kernel const&);
