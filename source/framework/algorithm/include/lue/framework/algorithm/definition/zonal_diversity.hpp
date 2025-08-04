#pragma once
#include "lue/framework/algorithm/definition/zonal_operation.hpp"
#include "lue/framework/algorithm/zonal_diversity.hpp"
#include "lue/framework/algorithm/zonal_operation_export.hpp"
#include <set>


namespace lue {
    namespace detail {

        template<typename Count, typename InputElement, typename Zone>
        class ZonalDiversity
        {

            public:

                static_assert(std::is_integral_v<Count>);
                static_assert(std::is_integral_v<InputElement>);
                static_assert(std::is_integral_v<Zone>);

                static constexpr char const* name{"zonal_diversity"};

                using OutputElement = Count;


                class Aggregator
                {

                    public:

                        using Statistic = std::set<InputElement>;

                        using Map = std::unordered_map<Zone, Statistic>;


                        void add(Zone const zone, InputElement const value)
                        {
                            _statistic_by_zone[zone].insert(value);
                        }


                        void merge(Aggregator const& other)
                        {
                            for (auto const& [zone, value] : other._statistic_by_zone)
                            {
                                if (!contains(zone))
                                {
                                    _statistic_by_zone[zone] = value;
                                }
                                else
                                {
                                    _statistic_by_zone[zone].insert(value.begin(), value.end());
                                }
                            }
                        }


                        bool contains(Zone const zone) const
                        {
                            return _statistic_by_zone.find(zone) != _statistic_by_zone.end();
                        }


                        OutputElement operator[](Zone const zone) const
                        {
                            auto it{_statistic_by_zone.find(zone)};

                            lue_hpx_assert(it != _statistic_by_zone.end());

                            return (*it).second.size();
                        }


                    private:

                        friend class hpx::serialization::access;


                        template<typename Archive>
                        void serialize(Archive& archive, unsigned int const /* version */)
                        {
                            archive & _statistic_by_zone;
                        }


                        Map _statistic_by_zone;
                };
        };

    }  // namespace detail


    /*!
      @brief      Return an array with per cell the number of unique element values found in the cells within
                  the corresponding zone
      @ingroup    zonal_operation
    */
    template<typename Count, typename Policies, typename Element, typename Zone, Rank rank>
    PartitionedArray<Count, rank> zonal_diversity(
        Policies const& policies,
        PartitionedArray<Element, rank> const& array,
        PartitionedArray<Zone, rank> const& zones)
    {
        using Functor = detail::ZonalDiversity<Count, Element, Zone>;

        return zonal_operation(policies, array, zones, Functor{});
    }

}  // namespace lue


#define LUE_INSTANTIATE_ZONAL_DIVERSITY(Policies, Count, Element, Zone)                                      \
                                                                                                             \
    template LUE_ZONAL_OPERATION_EXPORT PartitionedArray<Count, 2>                                           \
    zonal_diversity<Count, ArgumentType<void(Policies)>, Element, Zone, 2>(                                  \
        ArgumentType<void(Policies)> const&,                                                                 \
        PartitionedArray<Element, 2> const&,                                                                 \
        PartitionedArray<Zone, 2> const&);
