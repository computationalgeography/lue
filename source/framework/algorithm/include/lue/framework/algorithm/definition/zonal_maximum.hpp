#pragma once
#include "lue/framework/algorithm/zonal_maximum.hpp"
#include "lue/framework/algorithm/zonal_operation_export.hpp"
#include "lue/framework/algorithm/definition/zonal_operation.hpp"
#include <hpx/serialization/unordered_map.hpp>


namespace lue {
    namespace detail {

        template<
            typename InputElement,
            typename Zone>
        class ZonalMaximum
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
                            auto [it, _] = _maxima.try_emplace(zone, value);

                            (*it).second = std::max((*it).second, value);
                        }


                        void merge(
                            Aggregator const& other)
                        {
                            for(auto const& [zone, value]: other._maxima)
                            {
                                add(zone, value);
                            }
                        }


                        bool contains(
                            Zone const zone) const
                        {
                            return _maxima.find(zone) != _maxima.end();
                        }


                        InputElement operator[](
                            Zone const zone) const
                        {
                            auto it{_maxima.find(zone)};

                            lue_hpx_assert(it != _maxima.end());

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
                            archive & _maxima;
                        }


                        Map _maxima;

                };

        };

    }  // namespace detail


    template<
        typename Policies,
        typename Element,
        typename Zone,
        Rank rank>
    PartitionedArray<Element, rank> zonal_maximum(
        Policies const& policies,
        PartitionedArray<Element, rank> const& array,
        PartitionedArray<Zone, rank> const& zones)
    {
        using Functor = detail::ZonalMaximum<Element, Zone>;

        return zonal_operation(policies, array, zones, Functor{});
    }

}  // namespace lue


#define LUE_INSTANTIATE_ZONAL_MAXIMUM(                        \
    Policies, Element, Zone)                                  \
                                                              \
    template LUE_ZONAL_OPERATION_EXPORT                       \
    PartitionedArray<Element, 2> zonal_maximum<               \
            ArgumentType<void(Policies)>, Element, Zone, 2>(  \
        ArgumentType<void(Policies)> const&,                  \
        PartitionedArray<Element, 2> const&,                  \
        PartitionedArray<Zone, 2> const&);
