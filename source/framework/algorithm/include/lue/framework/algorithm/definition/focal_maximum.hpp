#pragma once
#include "lue/framework/algorithm/definition/focal_operation.hpp"
#include "lue/framework/algorithm/focal_maximum.hpp"
#include "lue/framework/algorithm/focal_operation_export.hpp"


namespace lue {
    namespace detail {

        template<Arithmetic Element>
        class FocalMaximum
        {

            public:

                static constexpr char const* name{"focal_maximum"};

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

                    OutputElement max{};
                    bool initialized{false};

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
                                    if (!initialized)
                                    {
                                        max = value;
                                        initialized = true;
                                    }
                                    else
                                    {
                                        max = std::max(max, value);
                                    }
                                }
                            }
                        }
                    }

                    if (!initialized)
                    {
                        ondp.mark_no_data(max);
                    }

                    return max;
                }
        };

    }  // namespace detail


    /*!
        @brief      Return an array with per cell the maximum element value found in the cells within the
                    corresponding focal neighbourhood
        @ingroup    focal_operation

        No-data values are filled unless all values within the neighbourhood are no-data.
    */
    template<typename Policies, typename Kernel>
        requires Arithmetic<policy::InputElementT<Policies, 0>> &&
                 Arithmetic<policy::OutputElementT<Policies, 0>> &&
                 std::same_as<policy::InputElementT<Policies, 0>, policy::OutputElementT<Policies, 0>> &&
                 std::integral<ElementT<Kernel>> && (rank<Kernel> == 2)
    auto focal_maximum(
        Policies const& policies,
        PartitionedArray<policy::InputElementT<Policies, 0>, 2> const& array,
        Kernel const& kernel) -> PartitionedArray<policy::OutputElementT<Policies, 0>, 2>
    {
        using Functor = detail::FocalMaximum<policy::InputElementT<Policies, 0>>;

        return focal_operation(policies, array, kernel, Functor{});
    }

}  // namespace lue


#define LUE_INSTANTIATE_FOCAL_MAXIMUM(Policies, Kernel)                                                      \
    template LUE_FOCAL_OPERATION_EXPORT auto focal_maximum<ArgumentType<void(Policies)>, Kernel>(            \
        ArgumentType<void(Policies)> const&,                                                                 \
        PartitionedArray<policy::InputElementT<Policies, 0>, 2> const&,                                      \
        Kernel const&) -> PartitionedArray<policy::OutputElementT<Policies, 0>, 2>;
