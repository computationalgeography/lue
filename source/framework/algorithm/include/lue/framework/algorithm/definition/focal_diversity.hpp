#pragma once
#include "lue/framework/algorithm/definition/focal_operation.hpp"
#include "lue/framework/algorithm/focal_diversity.hpp"
#include "lue/framework/algorithm/focal_operation_export.hpp"
#include <algorithm>
#include <set>


namespace lue {
    namespace detail {

        template<std::integral Count, std::integral Element>
        class FocalDiversity
        {

            public:

                using OutputElement = Count;
                using InputElement = Element;

                static constexpr char const* name{"focal_diversity"};


                template<typename Kernel, typename OutputPolicies, typename InputPolicies, typename Subspan>
                auto operator()(
                    Kernel const& kernel,
                    OutputPolicies const& output_policies,
                    InputPolicies const& input_policies,
                    Subspan const& window) const -> Count
                {
                    using Weight = ElementT<Kernel>;

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

                    for (Index idx0 = 0; idx0 < window.extent(0); ++idx0)
                    {
                        for (Index idx1 = 0; idx1 < window.extent(1); ++idx1)
                        {
                            Weight const weight{kernel(idx0, idx1)};
                            InputElement const value{window[idx0, idx1]};

                            if (!indp.is_no_data(value))
                            {
                                if (weight)
                                {
                                    values.push_back(value);
                                }
                            }
                        }
                    }

                    Count count{};

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
                  the corresponding focal neighbourhood
      @ingroup    focal_operation

      No-data values are filled unless all values within the neighbourhood are no-data.
    */
    template<typename Policies, typename Kernel>
        requires std::is_integral_v<policy::InputElementT<Policies, 0>> &&
                 std::is_integral_v<policy::OutputElementT<Policies, 0>> &&
                 std::is_integral_v<ElementT<Kernel>> && (rank<Kernel> == 2)
    auto focal_diversity(
        Policies const& policies,
        PartitionedArray<policy::InputElementT<Policies, 0>, 2> const& array,
        Kernel const& kernel) -> PartitionedArray<policy::OutputElementT<Policies, 0>, 2>
    {
        using Functor =
            detail::FocalDiversity<policy::OutputElementT<Policies, 0>, policy::InputElementT<Policies, 0>>;

        return focal_operation(policies, array, kernel, Functor{});
    }

}  // namespace lue


#define LUE_INSTANTIATE_FOCAL_DIVERSITY(Policies, Kernel)                                                    \
                                                                                                             \
    template LUE_FOCAL_OPERATION_EXPORT auto focal_diversity<ArgumentType<void(Policies)>, Kernel>(          \
        ArgumentType<void(Policies)> const&,                                                                 \
        PartitionedArray<policy::InputElementT<Policies, 0>, 2> const&,                                      \
        Kernel const&) -> PartitionedArray<policy::OutputElementT<Policies, 0>, 2>;
