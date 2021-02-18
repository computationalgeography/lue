#include "lue/py/framework/core/partitioned_array.hpp"


namespace lue::framework {

    void bind_partitioned_array_double(
        pybind11::module& module)
    {
        bind_partitioned_array<double, 2>(module);
    }

}  // namespace lue::framework
