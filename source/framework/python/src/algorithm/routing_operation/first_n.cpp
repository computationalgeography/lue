#include "lue/framework/algorithm/value_policies/first_n.hpp"
#include "bind.hpp"
#include "lue/framework/configure.hpp"


namespace lue::framework {
    namespace {

        class Binder
        {

            public:

                template<typename RouteID>
                static void bind(pybind11::module& module)
                {
                    Rank const rank{2};

                    module.def(
                        "first_n",
                        [](SerialRoute<RouteID, rank> const& route, Count const max_nr_cells)
                        { lue::value_policies::first_n<RouteID>(route, max_nr_cells); });
                }
        };

    }  // Anonymous namespace


    void bind_first_n(pybind11::module& module)
    {
        bind<Binder, IntegralElements>(module);
    }

}  // namespace lue::framework
