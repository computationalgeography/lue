#pragma once
#include "lue/framework/algorithm/focal_operation.hpp"


namespace lue {
namespace detail {

template<
    typename InputElement>
class FocalSum
{

public:

    using OutputElement = InputElement;

    FocalSum()=default;

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

                if constexpr(std::is_same_v<Weight, bool>)
                {
                    // TODO Handle no-data
                    if(weight)
                    {
                        sum += value;
                    }
                }
                else
                {
                    // TODO Handle no-data
                    sum += weight * value;
                }
            }
        }

        return sum;
    }

};

}  // namespace detail


template<
    typename Element,
    Rank rank,
    typename Kernel>
PartitionedArray<Element, rank> focal_sum(
    PartitionedArray<Element, rank> const& array,
    Kernel const& kernel)
{
    return focal_operation(array, kernel, detail::FocalSum<Element>{});
}

}  // namespace lue
