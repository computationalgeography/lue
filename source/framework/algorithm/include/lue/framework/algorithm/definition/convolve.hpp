#pragma once
#include "lue/framework/algorithm/convolve.hpp"
#include "lue/framework/algorithm/definition/focal_operation.hpp"


namespace lue {
    namespace detail {

        template<typename OutputElement_, typename InputElement>
        class Convolve
        {

            public:

                using OutputElement = OutputElement_;


                static_assert(std::is_convertible_v<InputElement, OutputElement>);


                Convolve() = default;


                template<typename Kernel, typename OutputPolicies, typename InputPolicies, typename Subspan>
                OutputElement operator()(
                    Kernel const& kernel,
                    [[maybe_unused]] OutputPolicies const& output_policies,
                    [[maybe_unused]] InputPolicies const& input_policies,
                    Subspan const& window) const
                {
                    static_assert(rank<Kernel> == 2);

                    using Weight = ElementT<Kernel>;

                    // TODO Add traits to grab typename of elements in Subspan
                    // static_assert(std::is_same_v<ElementT<Subspan>, InputElement>);
                    static_assert(std::is_same_v<Weight, bool> || std::is_floating_point_v<Weight>);

                    OutputElement sum{0};

                    lue_hpx_assert(static_cast<Count>(window.extent(0)) == kernel.size());
                    lue_hpx_assert(static_cast<Count>(window.extent(1)) == kernel.size());

                    for (Index r = 0; r < window.extent(0); ++r)
                    {
                        for (Index c = 0; c < window.extent(1); ++c)
                        {
                            Weight const weight{kernel(r, c)};
                            InputElement const value{window(r, c)};

                            if constexpr (std::is_same_v<Weight, bool>)
                            {
                                if (weight)
                                {
                                    sum += value;
                                }
                            }
                            else
                            {
                                sum += weight * value;
                            }
                        }
                    }

                    return sum;
                }
        };

    }  // namespace detail


    template<typename OutputElement, typename ConvolvePolicies, typename Array, typename Kernel>
    PartitionedArrayT<Array, OutputElement> convolve(
        ConvolvePolicies const& policies, Array const& array, Kernel const& kernel)
    {
        using Functor = detail::Convolve<OutputElement, ElementT<Array>>;

        return focal_operation(policies, array, kernel, Functor{});
    }

}  // namespace lue
