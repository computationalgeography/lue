#include "lue/framework/algorithm/value_policies/decreasing_order.hpp"
#include <pybind11/pybind11.h>


namespace lue::framework {

    namespace detail {

        template<typename ValueElement>
        void bind_decreasing_order(pybind11::module& module)
        {
            module.def("decreasing_order", lue::value_policies::decreasing_order<ValueElement, 2>);
        }


        template<typename ZoneElement, typename ValueElement>
        void bind_decreasing_order(pybind11::module& module)
        {
            module.def(
                "decreasing_order", lue::value_policies::decreasing_order<ZoneElement, ValueElement, 2>);
        }

    }  // namespace detail

    void bind_decreasing_order(pybind11::module& module)
    {
        // Global
        detail::bind_decreasing_order<std::uint8_t>(module);
        detail::bind_decreasing_order<std::int32_t>(module);
        detail::bind_decreasing_order<std::int64_t>(module);
        detail::bind_decreasing_order<std::uint32_t>(module);
        detail::bind_decreasing_order<std::uint64_t>(module);
        detail::bind_decreasing_order<float>(module);
        detail::bind_decreasing_order<double>(module);

        // Zonal
        detail::bind_decreasing_order<std::uint8_t, std::uint8_t>(module);
        detail::bind_decreasing_order<std::int32_t, std::uint8_t>(module);
        detail::bind_decreasing_order<std::int64_t, std::uint8_t>(module);
        detail::bind_decreasing_order<std::uint32_t, std::uint8_t>(module);
        detail::bind_decreasing_order<std::uint64_t, std::uint8_t>(module);

        detail::bind_decreasing_order<std::uint8_t, std::int32_t>(module);
        detail::bind_decreasing_order<std::int32_t, std::int32_t>(module);
        detail::bind_decreasing_order<std::int64_t, std::int32_t>(module);
        detail::bind_decreasing_order<std::uint32_t, std::int32_t>(module);
        detail::bind_decreasing_order<std::uint64_t, std::int32_t>(module);

        detail::bind_decreasing_order<std::uint8_t, std::int64_t>(module);
        detail::bind_decreasing_order<std::int32_t, std::int64_t>(module);
        detail::bind_decreasing_order<std::int64_t, std::int64_t>(module);
        detail::bind_decreasing_order<std::uint32_t, std::int64_t>(module);
        detail::bind_decreasing_order<std::uint64_t, std::int64_t>(module);

        detail::bind_decreasing_order<std::uint8_t, std::uint32_t>(module);
        detail::bind_decreasing_order<std::int32_t, std::uint32_t>(module);
        detail::bind_decreasing_order<std::int64_t, std::uint32_t>(module);
        detail::bind_decreasing_order<std::uint32_t, std::uint32_t>(module);
        detail::bind_decreasing_order<std::uint64_t, std::uint32_t>(module);

        detail::bind_decreasing_order<std::uint8_t, std::uint64_t>(module);
        detail::bind_decreasing_order<std::int32_t, std::uint64_t>(module);
        detail::bind_decreasing_order<std::int64_t, std::uint64_t>(module);
        detail::bind_decreasing_order<std::uint32_t, std::uint64_t>(module);
        detail::bind_decreasing_order<std::uint64_t, std::uint64_t>(module);

        detail::bind_decreasing_order<std::uint8_t, float>(module);
        detail::bind_decreasing_order<std::int32_t, float>(module);
        detail::bind_decreasing_order<std::int64_t, float>(module);
        detail::bind_decreasing_order<std::uint32_t, float>(module);
        detail::bind_decreasing_order<std::uint64_t, float>(module);

        detail::bind_decreasing_order<std::uint8_t, double>(module);
        detail::bind_decreasing_order<std::int32_t, double>(module);
        detail::bind_decreasing_order<std::int64_t, double>(module);
        detail::bind_decreasing_order<std::uint32_t, double>(module);
        detail::bind_decreasing_order<std::uint64_t, double>(module);
    }

}  // namespace lue::framework
