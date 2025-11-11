#pragma once
#include "lue/framework/algorithm/definition/focal_operation.hpp"
#include "lue/framework/algorithm/focal_minimum.hpp"
#include "lue/framework/algorithm/focal_operation_export.hpp"


namespace lue {
    namespace detail {

        template<Arithmetic Element>
        class FocalMinimum
        {

            public:

                static constexpr char const* name{"focal_minimum"};

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

                    OutputElement min{};
                    bool initialized{false};

                    for (Index r = 0; r < window.extent(0); ++r)
                    {
                        for (Index c = 0; c < window.extent(1); ++c)
                        {
                            Weight const weight{kernel(r, c)};
                            InputElement const value{window[r, c]};

                            if (!indp.is_no_data(value))
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


    /*!
        @brief      Return an array with per cell the minimum element value found in the cells within the
                    corresponding focal neighbourhood
        @ingroup    focal_operation

        No-data values are filled unless all values within the neighbourhood are no-data.
    */
    template<typename Policies, typename Kernel>
        requires Arithmetic<policy::InputElementT<Policies, 0>> &&
                 Arithmetic<policy::OutputElementT<Policies, 0>> &&
                 std::same_as<policy::InputElementT<Policies, 0>, policy::OutputElementT<Policies, 0>> &&
                 std::integral<ElementT<Kernel>> && (rank<Kernel> == 2)
    auto focal_minimum(
        Policies const& policies,
        PartitionedArray<policy::InputElementT<Policies, 0>, 2> const& array,
        Kernel const& kernel) -> PartitionedArray<policy::OutputElementT<Policies, 0>, 2>
    {
        using Functor = detail::FocalMinimum<policy::InputElementT<Policies, 0>>;

        return focal_operation(policies, array, kernel, Functor{});
    }

}  // namespace lue


#define LUE_INSTANTIATE_FOCAL_MINIMUM(Policies, Kernel)                                                      \
    template LUE_FOCAL_OPERATION_EXPORT auto focal_minimum<ArgumentType<void(Policies)>, Kernel>(            \
        ArgumentType<void(Policies)> const&,                                                                 \
        PartitionedArray<policy::InputElementT<Policies, 0>, 2> const&,                                      \
        Kernel const&) -> PartitionedArray<policy::OutputElementT<Policies, 0>, 2>;
