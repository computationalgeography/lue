#include "lue/py/framework/core/partitioned_array.hpp"


namespace lue::framework {

    void bind_partitioned_array_float(
        pybind11::module& module)
    {
        bind_partitioned_array<float, 2>(module);
    }

}  // namespace lue::framework
