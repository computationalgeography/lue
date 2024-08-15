#include "lue/py/framework/core/scalar.hpp"


namespace lue::framework {

    void bind_scalar(pybind11::module& module)
    {
        bind_scalar<std::uint8_t>(module);
        bind_scalar<std::uint32_t>(module);
        bind_scalar<std::uint64_t>(module);
        bind_scalar<std::int32_t>(module);
        bind_scalar<std::int64_t>(module);
        bind_scalar<float>(module);
        bind_scalar<double>(module);
    }

}  // namespace lue::framework
