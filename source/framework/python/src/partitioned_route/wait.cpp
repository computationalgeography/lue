#include "lue/framework/core/component.hpp"
#include "lue/framework/serial_route.hpp"
#include <pybind11/pybind11.h>


namespace lue::framework {
    namespace {

        template<typename RouteID, Rank rank>
        void wait(SerialRoute<RouteID, rank> const& route)
        {
            route.starts().wait();
        }

    }  // Anonymous namespace


    void bind_wait_serial_route(pybind11::module& module)
    {
        module.def("wait", wait<std::uint8_t, 2>);
        module.def("wait", wait<std::uint32_t, 2>);
        module.def("wait", wait<std::uint64_t, 2>);
        module.def("wait", wait<std::int32_t, 2>);
        module.def("wait", wait<std::int64_t, 2>);
    }
}  // namespace lue::framework
