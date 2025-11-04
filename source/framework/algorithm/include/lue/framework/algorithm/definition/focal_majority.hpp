#pragma once
#include "lue/framework/algorithm/definition/focal_operation.hpp"
#include "lue/framework/algorithm/focal_majority.hpp"
#include "lue/framework/algorithm/focal_operation_export.hpp"
#include <unordered_map>


namespace lue {
    namespace detail {

        template<std::integral Element>
        class FocalMajority
        {

            public:

                static constexpr char const* name{"focal_majority"};

                using InputElement = Element;
                using OutputElement = Element;


                template<typename Kernel, typename OutputPolicies, typename InputPolicies, typename Subspan>
                auto operator()(
                    Kernel const& kernel,
                    OutputPolicies const& output_policies,
                    InputPolicies const& input_policies,
                    Subspan const& window) const -> OutputElement
                {
                    using Weight = ElementT<Kernel>;

                    lue_hpx_assert(window.extent(0) == kernel.size());
                    lue_hpx_assert(window.extent(1) == kernel.size());

                    auto const& indp = input_policies.input_no_data_policy();
                    auto const& ondp = output_policies.output_no_data_policy();

                    std::map<InputElement, Count> frequencies;

                    for (Index idx0 = 0; idx0 < window.extent(0); ++idx0)
                    {
                        for (Index idx1 = 0; idx1 < window.extent(1); ++idx1)
                        {
                            InputElement const value{window[idx0, idx1]};

                            if (!indp.is_no_data(value))
                            {
                                Weight const weight{kernel(idx0, idx1)};

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

                        if (frequencies.empty())
                        {
                            ondp.mark_no_data(majority_value);
                        }
                        else
                        {
                            // Find value with largest frequency. If there are multiple, pick the smallest
                            // value.

                            for (auto const& [value, frequency] : frequencies)
                            {
                                if (frequency > majority_value_frequency)
                                {
                                    majority_value_frequency = frequency;
                                    majority_value = value;
                                }
                            }
                        }
                    }

                    return majority_value;
                }
        };

    }  // namespace detail


    /*!
        @brief      Return an array with per cell the most occurring element value found in the cells
                    within the corresponding focal neighbourhood
        @ingroup    focal_operation

        No-data values are filled unless all values within the neighbourhood are no-data.
    */
    template<typename Policies, typename Kernel>
        requires std::integral<policy::InputElementT<Policies, 0>> &&
                 std::integral<policy::OutputElementT<Policies, 0>> &&
                 std::same_as<policy::InputElementT<Policies, 0>, policy::OutputElementT<Policies, 0>> &&
                 std::integral<ElementT<Kernel>> && (rank<Kernel> == 2)
    auto focal_majority(
        Policies const& policies,
        PartitionedArray<policy::InputElementT<Policies, 0>, 2> const& array,
        Kernel const& kernel) -> PartitionedArray<policy::OutputElementT<Policies, 0>, 2>
    {
        using Functor = detail::FocalMajority<policy::OutputElementT<Policies, 0>>;

        return focal_operation(policies, array, kernel, Functor{});
    }

}  // namespace lue


#define LUE_INSTANTIATE_FOCAL_MAJORITY(Policies, Kernel)                                                     \
                                                                                                             \
    template LUE_FOCAL_OPERATION_EXPORT auto focal_majority<ArgumentType<void(Policies)>, Kernel>(           \
        ArgumentType<void(Policies)> const&,                                                                 \
        PartitionedArray<policy::InputElementT<Policies, 0>, 2> const&,                                      \
        Kernel const&) -> PartitionedArray<policy::OutputElementT<Policies, 0>, 2>;
