#pragma once
#include "lue/framework/algorithm/zonal_operation.hpp"
#include <hpx/serialization/unordered_map.hpp>


namespace lue {
namespace detail {

template<
    typename InputElement,
    typename Zone>
class ZonalSum
{

public:

    static_assert(std::is_arithmetic_v<InputElement>);

    static_assert(std::is_integral_v<Zone>);

    using OutputElement = InputElement;

    class Aggregator
    {

    public:

        using Map = std::unordered_map<Zone, OutputElement>;

        void add(
            Zone const zone,
            InputElement const value)
        {
            if(_sums.find(zone) == _sums.end())
            {
                _sums[zone] = value;
            }
            else
            {
                _sums[zone] += value;
            }
        }

        void merge(
            Aggregator const& other)
        {
            for(auto const& [zone, value]: other._sums)
            {
                add(zone, value);
            }
        }

        InputElement operator[](
            Zone const zone) const
        {
            auto it{_sums.find(zone)};

            lue_assert(it != _sums.end());

            return (*it).second;
        }

    private:

        friend class hpx::serialization::access;

        template<
            typename Archive>
        void serialize(
            Archive& archive,
            unsigned int const /* version */)
        {
            archive & _sums;
        }

        Map _sums;

    };

};

}  // namespace detail


template<
    typename Element,
    typename Zone,
    Rank rank>
PartitionedArray<Element, rank> zonal_sum(
    PartitionedArray<Element, rank> const& array,
    PartitionedArray<Zone, rank> const& zones)
{
    return zonal_operation(array, zones, detail::ZonalSum<Element, Zone>{});
}

}  // namespace lue
