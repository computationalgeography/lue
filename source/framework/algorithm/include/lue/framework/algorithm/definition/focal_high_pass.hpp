#pragma once
#include "lue/framework/algorithm/definition/focal_operation.hpp"
#include "lue/framework/algorithm/focal_high_pass.hpp"
#include "lue/framework/algorithm/focal_operation_export.hpp"


namespace lue {
    namespace detail {

        template<std::floating_point Element>
        class FocalHighPass
        {

            public:

                static constexpr char const* name{"focal_high_pass"};

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

                    Radius const radius{kernel.radius()};

                    lue_hpx_assert(window.extent(0) == kernel.size());
                    lue_hpx_assert(window.extent(1) == kernel.size());

                    auto const& indp = input_policies.input_no_data_policy();
                    auto const& ondp = output_policies.output_no_data_policy();

                    InputElement focal_value{};
                    InputElement sum_of_neighbours{0};
                    Count nr_neighbours{0};

                    for (Index idx0 = 0; idx0 < window.extent(0); ++idx0)
                    {
                        for (Index idx1 = 0; idx1 < window.extent(1); ++idx1)
                        {
                            InputElement const value{window[idx0, idx1]};

                            if (idx0 == radius && idx1 == radius)
                            {
                                // Remember the focal value. Its kernel weight is irrelevant.
                                focal_value = value;
                            }
                            else
                            {
                                if (!indp.is_no_data(value))
                                {
                                    Weight const weight{kernel(idx0, idx1)};

                                    if (weight)
                                    {
                                        sum_of_neighbours += value;
                                        ++nr_neighbours;
                                    }
                                }
                            }
                        }
                    }

                    OutputElement high_pass{};

                    {
                        if (nr_neighbours == 0)
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


    /*!
        @brief      .
        @tparam     .
        @param      .
        @return     .
        @exception  .
        @ingroup    focal_operation

        No-data in input focal cells are propagated to output focal cells. No-data values in neighbourhood
        cells are skipped.
    */
    template<typename Policies, typename Kernel>
        requires std::floating_point<policy::InputElementT<Policies, 0>> &&
                 std::floating_point<policy::OutputElementT<Policies, 0>> &&
                 std::same_as<policy::InputElementT<Policies, 0>, policy::OutputElementT<Policies, 0>> &&
                 std::integral<ElementT<Kernel>> && (rank<Kernel> == 2)
    auto focal_high_pass(
        Policies const& policies,
        PartitionedArray<policy::InputElementT<Policies, 0>, 2> const& array,
        Kernel const& kernel) -> PartitionedArray<policy::OutputElementT<Policies, 0>, 2>
    {
        using Element = policy::InputElementT<Policies, 0>;
        using Functor = detail::FocalHighPass<Element>;

        return focal_operation(policies, array, kernel, Functor{});
    }

}  // namespace lue


#define LUE_INSTANTIATE_FOCAL_HIGH_PASS(Policies, Kernel)                                                    \
                                                                                                             \
    template LUE_FOCAL_OPERATION_EXPORT auto focal_high_pass<ArgumentType<void(Policies)>, Kernel>(          \
        ArgumentType<void(Policies)> const&,                                                                 \
        PartitionedArray<policy::InputElementT<Policies, 0>, 2> const&,                                      \
        Kernel const&) -> PartitionedArray<policy::OutputElementT<Policies, 0>, 2>;
