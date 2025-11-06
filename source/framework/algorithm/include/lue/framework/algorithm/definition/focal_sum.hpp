#pragma once
#include "lue/framework/algorithm/definition/focal_operation.hpp"
#include "lue/framework/algorithm/focal_operation_export.hpp"
#include "lue/framework/algorithm/focal_sum.hpp"


namespace lue {
    namespace detail {

        template<Arithmetic Element>
        class FocalSum
        {

            public:

                static constexpr char const* name{"focal_sum"};

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

                    OutputElement sum{0};
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
                                    // TODO: Use out-of-range policy to see whether this succeeds.
                                    //       (Same policy as used by addition.)
                                    sum += value;
                                    initialized = true;
                                }
                            }
                        }
                    }

                    if (!initialized)
                    {
                        ondp.mark_no_data(sum);
                    }

                    return sum;
                }
        };

    }  // namespace detail


    /*!
        @brief      Return an array with per cell the sum of all values within the corresponding focal
                    neighbourhood
        @ingroup    focal_operation

        No-data values are filled unless all values within the neighbourhood are no-data.
    */
    template<typename Policies, typename Kernel>
        requires Arithmetic<policy::InputElementT<Policies, 0>> &&
                 Arithmetic<policy::OutputElementT<Policies, 0>> &&
                 std::same_as<policy::InputElementT<Policies, 0>, policy::OutputElementT<Policies, 0>> &&
                 std::integral<ElementT<Kernel>> && (rank<Kernel> == 2)
    auto focal_sum(
        Policies const& policies,
        PartitionedArray<policy::InputElementT<Policies, 0>, 2> const& array,
        Kernel const& kernel) -> PartitionedArray<policy::OutputElementT<Policies, 0>, 2>
    {
        using Functor = detail::FocalSum<policy::InputElementT<Policies, 0>>;

        return focal_operation(policies, array, kernel, Functor{});
    }

}  // namespace lue


#define LUE_INSTANTIATE_FOCAL_SUM(Policies, Kernel)                                                          \
    template LUE_FOCAL_OPERATION_EXPORT auto focal_sum<ArgumentType<void(Policies)>, Kernel>(                \
        ArgumentType<void(Policies)> const&,                                                                 \
        PartitionedArray<policy::InputElementT<Policies, 0>, 2> const&,                                      \
        Kernel const&) -> PartitionedArray<policy::OutputElementT<Policies, 0>, 2>;
