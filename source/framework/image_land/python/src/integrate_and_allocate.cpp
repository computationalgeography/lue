#include "lue/framework/algorithm/value_policies/integrate_and_allocate.hpp"
#include <pybind11/stl.h>


namespace lue::image_land {
    namespace detail {

        template<typename ZoneElement, typename FloatingPointElement, Rank rank>
        auto integrate_and_allocate(
            SerialRoute<ZoneElement, rank> const& route,
            std::vector<std::reference_wrapper<PartitionedArray<FloatingPointElement, rank> const>> const&
                sdp_factors,
            std::vector<std::reference_wrapper<PartitionedArray<FloatingPointElement, rank> const>> const&
                yield_factors,
            std::vector<std::reference_wrapper<PartitionedArray<FloatingPointElement, rank> const>> const&
                crop_fractions,
            hpx::shared_future<std::map<ZoneElement, std::vector<FloatingPointElement>>> const& demands,
            hpx::shared_future<std::map<ZoneElement, std::vector<FloatingPointElement>>> const&
                current_production,
            PartitionedArray<FloatingPointElement, rank> const& irrigated_crop_fractions)
        {
            return lue::value_policies::integrate_and_allocate(
                route,
                sdp_factors,
                yield_factors,
                crop_fractions,
                demands,
                current_production,
                irrigated_crop_fractions);
        }


        template<typename ZoneElement, typename FloatingPointElement, Rank rank>
        void bind_integrate_and_allocate(pybind11::module& module)
        {
            module.def(
                "integrate_and_allocate",
                lue::image_land::detail::integrate_and_allocate<ZoneElement, FloatingPointElement, rank>,
                pybind11::return_value_policy::move);
        }

    }  // namespace detail


    void bind_integrate_and_allocate(pybind11::module& module)
    {
        Rank const rank{2};

        detail::bind_integrate_and_allocate<std::uint8_t, float, rank>(module);
        detail::bind_integrate_and_allocate<std::uint32_t, float, rank>(module);
        detail::bind_integrate_and_allocate<std::uint64_t, float, rank>(module);
        detail::bind_integrate_and_allocate<std::int32_t, float, rank>(module);
        detail::bind_integrate_and_allocate<std::int64_t, float, rank>(module);

        detail::bind_integrate_and_allocate<std::uint8_t, double, rank>(module);
        detail::bind_integrate_and_allocate<std::uint32_t, double, rank>(module);
        detail::bind_integrate_and_allocate<std::uint64_t, double, rank>(module);
        detail::bind_integrate_and_allocate<std::int32_t, double, rank>(module);
        detail::bind_integrate_and_allocate<std::int64_t, double, rank>(module);
    }

}  // namespace lue::image_land
