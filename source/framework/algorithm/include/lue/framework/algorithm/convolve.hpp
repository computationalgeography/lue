#pragma once
#include "lue/framework/algorithm/focal_operation.hpp"


namespace lue {
namespace detail {

template<
    typename InputElement,
    typename OutputElement_>
class Convolve
{

public:

    using OutputElement = OutputElement_;

    static_assert(std::is_convertible_v<InputElement, OutputElement>);

    Convolve()=default;

    constexpr InputElement fill_value() const
    {
        return 0;
    }

    template<
        typename Subspan,
        typename Kernel>
    OutputElement operator()(
        Subspan const& window,
        Kernel const& kernel) const
    {
        static_assert(rank<Kernel> == 2);

        using Weight = ElementT<Kernel>;

        // TODO Add traits to grab typename of elements in Subspan
        // static_assert(std::is_same_v<ElementT<Subspan>, InputElement>);
        static_assert(std::is_same_v<Weight, bool> || std::is_floating_point_v<Weight>);

        OutputElement sum{0};

        lue_assert(window.extent(0) == kernel.size());
        lue_assert(window.extent(1) == kernel.size());

        for(Index r = 0; r < window.extent(0); ++r) {
            for(Index c = 0; c < window.extent(1); ++c)
            {
                Weight const weight{kernel(r, c)};
                InputElement const value{window(r, c)};

                if constexpr(std::is_same_v<Weight, bool>) {
                    if(weight) {
                        sum += value;
                    }
                }
                else {
                    sum += weight * value;
                }
            }
        }

        return sum;
    }
};

}  // namespace detail


template<
    typename OutOfImagePolicy_=SkipOutOfImage,
    typename NoDataFocusElementPolicy_=KeepNoDataFocusElement
>
class ConvolvePolicies:
    public OutOfImagePolicy_,
    public NoDataFocusElementPolicy_
{

public:

    using OutOfImagePolicy = OutOfImagePolicy_;

    using NoDataFocusElementPolicy = NoDataFocusElementPolicy_;

    ConvolvePolicies() {};

private:

};


template<
    typename ConvolvePolicies,
    typename Array,
    typename Kernel,
    typename OutputElement=double>
PartitionedArrayT<Array, OutputElement> convolve(
    ConvolvePolicies const& policies,
    Array const& array,
    Kernel const& kernel)
{
    return focal_operation(
        policies, array, kernel, detail::Convolve<ElementT<Array>,
        OutputElement>{});
}


template<
    typename Array,
    typename Kernel,
    typename OutputElement=double>
PartitionedArrayT<Array, OutputElement> convolve(
    Array const& array,
    Kernel const& kernel)
{
    return convolve(ConvolvePolicies{}, array, kernel);
}

}  // namespace lue
