#include "lue/framework/serial_route.hpp"
#include "lue/py/configure.hpp"
#include "lue/py/framework/stream.hpp"
#include <pybind11/stl.h>


namespace lue::framework {

    namespace detail {

        template<Rank rank>
        auto formal_string_representation(SerialRoute<rank> const& route) -> std::string
        {
            return fmt::format(
                "SerialRoute<{}>{{shape={}, shape_in_partitions={}}}",
                rank,
                as_string(route.shape()),
                as_string(route.partitions().shape()));
        }


        template<Rank rank>
        auto informal_string_representation(SerialRoute<rank> const& route) -> std::string
        {
            return formal_string_representation(route);
        }


        template<Rank rank>
        void bind_serial_route(pybind11::module& module)
        {
            namespace lfr = lue::framework;

            using Route = SerialRoute<rank>;

            auto class_ =
                pybind11::class_<Route>(
                    module,
                    fmt::format("SerialRoute_{}", rank).c_str(),
                    fmt::format(
                        R"(
    Partitioned serial route type for routes of rank {}
)",
                        rank)
                        .c_str())

                    .def_property_readonly(
                        "shape",
                        [](Route const& self)
                        {
                            static_assert(rank == 2);

                            auto shape{self.shape()};

                            return std::make_tuple(shape[0], shape[1]);
                        },
                        "shape docstring...")

                    .def_property_readonly(
                        "shape_in_partitions",
                        [](Route const& self)
                        {
                            static_assert(rank == 2);

                            auto shape{self.partitions().shape()};

                            return std::make_tuple(shape[0], shape[1]);
                        },
                        "shape_in_partitions docstring...")

                    .def(
                        "__repr__",
                        [](Route const& route) { return detail::formal_string_representation(route); })

                    .def(
                        "__str__",
                        [](Route const& route) { return detail::informal_string_representation(route); })

                ;
        }

    }  // namespace detail


    void bind_serial_route(pybind11::module& module)
    {
        detail::bind_serial_route<2>(module);
    }

}  // namespace lue::framework
