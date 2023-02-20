#include "lue/py/framework/core/partitioned_array.hpp"


namespace lue::framework {

    void bind_partitioned_array_int32(pybind11::module& module)
    {
        bind_partitioned_array<std::int32_t, 2>(module);
    }

}  // namespace lue::framework
