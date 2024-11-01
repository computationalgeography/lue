#include "lue/framework/core/component.hpp"
#include "lue/framework/serial_route.hpp"
#include "lue/concept.hpp"
#include "lue/framework.hpp"
#include <pybind11/pybind11.h>


namespace lue::framework {
    namespace {

        template<typename RouteID, Rank rank>
        void wait(SerialRoute<RouteID, rank> const& route)
        {
            route.starts().wait();
        }


        template<Arithmetic Element>
        void bind(pybind11::module& module)
        {
            Rank const rank{2};

            module.def("wait", wait<Element, rank>);
        }


        template<TupleLike Elements, std::size_t idx>
        void bind(pybind11::module& module) requires(idx == 0)
        {
            bind<std::tuple_element_t<idx, Elements>>(module);
        }


        template<TupleLike Elements, std::size_t idx>
        void bind(pybind11::module& module) requires(idx > 0)
        {
            bind<std::tuple_element_t<idx, Elements>>(module);
            bind<Elements, idx - 1>(module);
        }


        template<TupleLike Elements>
        void bind(pybind11::module& module)
        {
            bind<Elements, std::tuple_size_v<Elements> - 1>(module);
        }

    }  // Anonymous namespace


    void bind_wait_serial_route(pybind11::module& module)
    {
        bind<ZoneElements>(module);
    }
}  // namespace lue::framework
