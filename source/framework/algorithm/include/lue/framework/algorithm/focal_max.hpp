#pragma once
#include "lue/framework/algorithm/focal_operation.hpp"
#include <limits>


namespace lue {
namespace detail {

template<
    typename InputElement>
class FocalMax
{

public:

    using OutputElement = InputElement;

    FocalMax()=default;

    constexpr InputElement fill_value() const
    {
        return std::numeric_limits<InputElement>::min();
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
        static_assert(std::is_convertible_v<Weight, bool>);

        OutputElement max{};
        bool initialized{false};

        lue_assert(window.extent(0) == kernel.size());
        lue_assert(window.extent(1) == kernel.size());

        for(Index r = 0; r < window.extent(0); ++r) {
            for(Index c = 0; c < window.extent(1); ++c)
            {
                Weight const weight{kernel(r, c)};
                InputElement const value{window(r, c)};

                if(weight)
                {
                    if(!initialized)
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

        // TODO(KDJ)
        // If the kernel weight are all false, or when no-data are
        // supported and all values are no-data, then the result is not
        // initialized. In that case we must mark the result to no-data.
        assert(initialized);

        return max;
    }

};

}  // namespace detail


template<
    typename Element,
    Rank rank,
    typename Kernel>
PartitionedArray<Element, rank> focal_max(
    PartitionedArray<Element, rank> const& array,
    Kernel const& kernel)
{
    return focal_operation(array, kernel, detail::FocalMax<Element>{});
}

}  // namespace lue
