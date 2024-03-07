#include "lue/framework/algorithm/value_policies/decreasing_order.hpp"
#include <pybind11/pybind11.h>


namespace lue::framework {

    namespace detail {

        template<typename RouteID, typename FieldElement, Rank rank>
        auto decreasing_order_global(
            PartitionedArray<FieldElement, rank> const& value, Count const max_nr_cells)
        {
            return lue::value_policies::decreasing_order<RouteID>(value, max_nr_cells);
        }


        template<typename ZoneElement, typename FieldElement, Rank rank>
        auto decreasing_order_zonal(
            PartitionedArray<ZoneElement, rank> const& zone,
            PartitionedArray<FieldElement, rank> const& value,
            Count const max_nr_cells)
        {
            return lue::value_policies::decreasing_order(zone, value, max_nr_cells);
        }


        template<typename ZoneElement, typename FieldElement>
        void bind_decreasing_order(pybind11::module& module)
        {
            using namespace pybind11::literals;
            using RouteID = ZoneElement;
            Rank const rank{2};

            module.def(
                "decreasing_order",
                decreasing_order_global<RouteID, FieldElement, rank>,
                "value"_a,
                pybind11::kw_only(),
                "max_nr_cells"_a = std::numeric_limits<Count>::max());
            module.def(
                "decreasing_order",
                decreasing_order_zonal<ZoneElement, FieldElement, rank>,
                "zone"_a,
                "value"_a,
                pybind11::kw_only(),
                "max_nr_cells"_a = std::numeric_limits<Count>::max());
        }

    }  // namespace detail

    void bind_decreasing_order(pybind11::module& module)
    {
        detail::bind_decreasing_order<std::uint8_t, std::uint8_t>(module);
        detail::bind_decreasing_order<std::int32_t, std::uint8_t>(module);
        detail::bind_decreasing_order<std::int64_t, std::uint8_t>(module);
        detail::bind_decreasing_order<std::uint32_t, std::uint8_t>(module);
        detail::bind_decreasing_order<std::uint64_t, std::uint8_t>(module);

        detail::bind_decreasing_order<std::uint8_t, std::int32_t>(module);
        detail::bind_decreasing_order<std::int32_t, std::int32_t>(module);
        detail::bind_decreasing_order<std::int64_t, std::int32_t>(module);
        detail::bind_decreasing_order<std::uint32_t, std::int32_t>(module);
        detail::bind_decreasing_order<std::uint64_t, std::int32_t>(module);

        detail::bind_decreasing_order<std::uint8_t, std::int64_t>(module);
        detail::bind_decreasing_order<std::int32_t, std::int64_t>(module);
        detail::bind_decreasing_order<std::int64_t, std::int64_t>(module);
        detail::bind_decreasing_order<std::uint32_t, std::int64_t>(module);
        detail::bind_decreasing_order<std::uint64_t, std::int64_t>(module);

        detail::bind_decreasing_order<std::uint8_t, std::uint32_t>(module);
        detail::bind_decreasing_order<std::int32_t, std::uint32_t>(module);
        detail::bind_decreasing_order<std::int64_t, std::uint32_t>(module);
        detail::bind_decreasing_order<std::uint32_t, std::uint32_t>(module);
        detail::bind_decreasing_order<std::uint64_t, std::uint32_t>(module);

        detail::bind_decreasing_order<std::uint8_t, std::uint64_t>(module);
        detail::bind_decreasing_order<std::int32_t, std::uint64_t>(module);
        detail::bind_decreasing_order<std::int64_t, std::uint64_t>(module);
        detail::bind_decreasing_order<std::uint32_t, std::uint64_t>(module);
        detail::bind_decreasing_order<std::uint64_t, std::uint64_t>(module);

        detail::bind_decreasing_order<std::uint8_t, float>(module);
        detail::bind_decreasing_order<std::int32_t, float>(module);
        detail::bind_decreasing_order<std::int64_t, float>(module);
        detail::bind_decreasing_order<std::uint32_t, float>(module);
        detail::bind_decreasing_order<std::uint64_t, float>(module);

        detail::bind_decreasing_order<std::uint8_t, double>(module);
        detail::bind_decreasing_order<std::int32_t, double>(module);
        detail::bind_decreasing_order<std::int64_t, double>(module);
        detail::bind_decreasing_order<std::uint32_t, double>(module);
        detail::bind_decreasing_order<std::uint64_t, double>(module);
    }

}  // namespace lue::framework
