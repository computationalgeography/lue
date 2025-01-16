#include "lue/framework/algorithm/value_policies/integrate.hpp"
#include "lue/framework.hpp"
#include "lue/py/bind.hpp"


namespace lue::image_land {
    namespace {

        class Binder
        {

            public:

                template<std::integral RouteID, std::floating_point IntegrandElement>
                static void bind(pybind11::module& module)
                {
                    using namespace pybind11::literals;

                    Rank const rank{2};

                    module.def(
                        "integrate",
                        [](SerialRoute<RouteID, rank> const& route,
                           PartitionedArray<IntegrandElement, rank> const& integrand,
                           Count const max_nr_cells) -> PartitionedArray<IntegrandElement, rank>
                        { return lue::value_policies::integrate(route, integrand, max_nr_cells); },
                        "route"_a,
                        "integrand"_a,
                        pybind11::kw_only(),
                        "max_nr_cells"_a = std::numeric_limits<Count>::max());
                }
        };

    }  // Anonymous namespace


    void bind_integrate(pybind11::module& module)
    {
        framework::bind<Binder, ZoneElements, FloatingPointElements>(module);
    }

}  // namespace lue::image_land
