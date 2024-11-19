#include "lue/framework/algorithm/value_policies/integrate_and_allocate.hpp"
#include "bind.hpp"
#include "lue/framework.hpp"
#include <pybind11/stl.h>


namespace lue::image_land {
    namespace {

        class Binder
        {

            public:

                template<std::integral ZoneElement, std::floating_point FloatingPointElement>
                static void bind(pybind11::module& module)
                {
                    using namespace pybind11::literals;

                    Rank const rank{2};

                    module.def(
                        "integrate_and_allocate",
                        [](SerialRoute<ZoneElement, rank> const& route,
                           std::vector<std::reference_wrapper<
                               PartitionedArray<FloatingPointElement, rank> const>> const& sdp_factors,
                           std::vector<std::reference_wrapper<
                               PartitionedArray<FloatingPointElement, rank> const>> const& yield_factors,
                           std::vector<std::reference_wrapper<
                               PartitionedArray<FloatingPointElement, rank> const>> const& crop_fractions,
                           hpx::shared_future<std::map<ZoneElement, std::vector<FloatingPointElement>>> const&
                               demands,
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
                        },
                        pybind11::return_value_policy::move);
                }
        };

    }  // Anonymous namespace


    void bind_integrate_and_allocate(pybind11::module& module)
    {
        framework::bind<Binder, ZoneElements, FloatingPointElements>(module);
    }

}  // namespace lue::image_land
