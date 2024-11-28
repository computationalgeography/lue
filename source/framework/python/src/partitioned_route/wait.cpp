#include "lue/framework/core/component.hpp"
#include "lue/framework/serial_route.hpp"
#include "lue/framework.hpp"
#include "lue/py/bind.hpp"


namespace lue::framework {
    namespace {

        template<typename RouteID, Rank rank>
        void wait(SerialRoute<RouteID, rank> const& route)
        {
            route.starts().wait();
        }


        class Binder
        {

            public:

                template<Arithmetic Element>
                static void bind(pybind11::module& module)
                {
                    Rank const rank{2};

                    module.def("wait", wait<Element, rank>);
                }
        };

    }  // Anonymous namespace


    void bind_wait_serial_route(pybind11::module& module)
    {
        bind<Binder, ZoneElements>(module);
    }
}  // namespace lue::framework
