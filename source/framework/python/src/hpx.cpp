#include <hpx/future.hpp>
#include "core/type_traits.hpp"
#include <pybind11/pybind11.h>
#include <fmt/format.h>


namespace lue {
namespace framework {
namespace {

template<
    typename Element>
void bind_future(
    pybind11::module& module)
{
    pybind11::class_<hpx::future<Element>>(
        module,
        fmt::format("future<{}>", as_string<Element>()).c_str())

        ;

}


template<
    typename Element>
void bind_shared_future(
    pybind11::module& module)
{
    pybind11::class_<hpx::shared_future<Element>>(
        module,
        fmt::format("shared_future<{}>", as_string<Element>()).c_str())

        .def(
            "get",
            pybind11::overload_cast<>(&hpx::shared_future<Element>::get, pybind11::const_),
            // Copy to keep things simple. Use const reference in case Element can be big.
            pybind11::return_value_policy::copy)

        ;

}

}  // Anonymous namespace


void bind_hpx(
    pybind11::module& module)
{
    bind_future<std::uint8_t>(module);
    bind_future<std::uint32_t>(module);
    bind_future<std::uint64_t>(module);
    bind_future<std::int32_t>(module);
    bind_future<std::int64_t>(module);
    bind_future<float>(module);
    bind_future<double>(module);

    bind_shared_future<std::uint8_t>(module);
    bind_shared_future<std::uint32_t>(module);
    bind_shared_future<std::uint64_t>(module);
    bind_shared_future<std::int32_t>(module);
    bind_shared_future<std::int64_t>(module);
    bind_shared_future<float>(module);
    bind_shared_future<double>(module);
}

}  // namespace framework
}  // namespace lue
