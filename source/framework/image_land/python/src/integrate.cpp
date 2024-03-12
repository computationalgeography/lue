#include "lue/framework/algorithm/value_policies/integrate.hpp"
#include <pybind11/pybind11.h>


namespace lue::image_land {
    namespace detail {

        template<typename RouteID, typename IntegrandElement, Rank rank>
        auto integrate(
            SerialRoute<RouteID, rank> const& route,
            PartitionedArray<IntegrandElement, rank> const& integrand,
            Count const max_nr_cells) -> PartitionedArray<IntegrandElement, rank>
        {
            return lue::value_policies::integrate(route, integrand, max_nr_cells);
        }


        template<typename RouteID, typename IntegrandElement, Rank rank>
        void bind_integrate(pybind11::module& module)
        {
            using namespace pybind11::literals;

            module.def(
                "integrate",
                integrate<RouteID, IntegrandElement, rank>,
                "route"_a,
                "integrand"_a,
                pybind11::kw_only(),
                "max_nr_cells"_a = std::numeric_limits<Count>::max());
        }

    }  // namespace detail

    void bind_integrate(pybind11::module& module)
    {
        Rank const rank{2};

        detail::bind_integrate<std::uint8_t, float, rank>(module);
        detail::bind_integrate<std::uint32_t, float, rank>(module);
        detail::bind_integrate<std::uint64_t, float, rank>(module);
        detail::bind_integrate<std::int32_t, float, rank>(module);
        detail::bind_integrate<std::int64_t, float, rank>(module);

        detail::bind_integrate<std::uint8_t, double, rank>(module);
        detail::bind_integrate<std::uint32_t, double, rank>(module);
        detail::bind_integrate<std::uint64_t, double, rank>(module);
        detail::bind_integrate<std::int32_t, double, rank>(module);
        detail::bind_integrate<std::int64_t, double, rank>(module);
    }

}  // namespace lue::image_land
