#include "lue/framework/algorithm/value_policies/decreasing_order.hpp"
#include "bind.hpp"
#include "lue/framework/configure.hpp"


namespace lue::framework {
    namespace {

        using namespace pybind11::literals;

        class Binder
        {

            public:

                template<std::integral ZoneElement, Arithmetic ValueElement>
                static void bind(pybind11::module& module)
                {
                    Rank const rank{2};

                    module.def(
                        "decreasing_order",
                        [](PartitionedArray<ValueElement, rank> const& value, Count const max_nr_cells)
                        {
                            // Global / one zone
                            return lue::value_policies::decreasing_order<ZoneElement>(value, max_nr_cells);
                        },
                        "value"_a,
                        pybind11::kw_only(),
                        "max_nr_cells"_a = std::numeric_limits<Count>::max());
                    module.def(
                        "decreasing_order",
                        [](PartitionedArray<ZoneElement, rank> const& zone,
                           PartitionedArray<ValueElement, rank> const& value,
                           Count const max_nr_cells)
                        {
                            // Zonal
                            return lue::value_policies::decreasing_order(zone, value, max_nr_cells);
                        },
                        "zone"_a,
                        "value"_a,
                        pybind11::kw_only(),
                        "max_nr_cells"_a = std::numeric_limits<Count>::max());
                }
        };

    }  // Anonymous namespace

    void bind_decreasing_order(pybind11::module& module)
    {
        bind<Binder, ZoneElements, ArithmeticElements>(module);
    }

}  // namespace lue::framework
