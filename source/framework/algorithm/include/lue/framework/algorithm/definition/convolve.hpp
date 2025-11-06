#pragma once
#include "lue/framework/algorithm/convolve.hpp"
#include "lue/framework/algorithm/definition/focal_operation.hpp"
#include "lue/framework/algorithm/focal_operation_export.hpp"


namespace lue {
    namespace detail {

        template<std::floating_point Element>
        class Convolve
        {

            public:

                static constexpr char const* name{"convolve"};

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
                            InputElement const value{window[idx0, idx1]};

                            if (!indp.is_no_data(value))
                            {
                                Weight const weight{kernel(idx0, idx1)};

                                if constexpr (std::is_integral_v<Weight>)
                                {
                                    if (weight)
                                    {
                                        sum += value;
                                        initialized = true;
                                    }
                                }
                                else
                                {
                                    sum += weight * value;
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
        @brief      Return an array with the convolution of @a array with the weights of @a kernel
        @ingroup    focal_operation

        For each focal cell the input cells within the neighbourhood are multiplied by the corresponding
        weight and summed.

        No-data values are filled unless all values within the neighbourhood are no-data.

        See focal_sum() for an algorithm that sums values using boolean weights (represented by integrals).
    */
    template<typename Policies, typename Kernel>
        requires std::floating_point<policy::InputElementT<Policies, 0>> &&
                 std::floating_point<policy::OutputElementT<Policies, 0>> &&
                 std::same_as<policy::InputElementT<Policies, 0>, policy::OutputElementT<Policies, 0>> &&
                 std::floating_point<ElementT<Kernel>> && (rank<Kernel> == 2)
    auto convolve(
        Policies const& policies,
        PartitionedArray<policy::InputElementT<Policies, 0>, 2> const& array,
        Kernel const& kernel) -> PartitionedArray<policy::OutputElementT<Policies, 0>, 2>
    {
        using Functor = detail::Convolve<policy::InputElementT<Policies, 0>>;

        return focal_operation(policies, array, kernel, Functor{});
    }

}  // namespace lue


#define LUE_INSTANTIATE_CONVOLVE(Policies, Kernel)                                                           \
    template LUE_FOCAL_OPERATION_EXPORT auto convolve<ArgumentType<void(Policies)>, Kernel>(                 \
        ArgumentType<void(Policies)> const&,                                                                 \
        PartitionedArray<policy::InputElementT<Policies, 0>, 2> const&,                                      \
        Kernel const&) -> PartitionedArray<policy::OutputElementT<Policies, 0>, 2>;
