#include "lue/py/framework/core/partitioned_array.hpp"


namespace lue::framework {

    void bind_partitioned_array_uint8(pybind11::module& module);
    void bind_partitioned_array_uint32(pybind11::module& module);
    void bind_partitioned_array_uint64(pybind11::module& module);
    void bind_partitioned_array_int32(pybind11::module& module);
    void bind_partitioned_array_int64(pybind11::module& module);
    void bind_partitioned_array_float(pybind11::module& module);
    void bind_partitioned_array_double(pybind11::module& module);


    void bind_partitioned_array(
        pybind11::module& module)
    {
        bind_partitioned_array_uint8(module);
        bind_partitioned_array_uint32(module);
        bind_partitioned_array_uint64(module);
        bind_partitioned_array_int32(module);
        bind_partitioned_array_int64(module);
        bind_partitioned_array_float(module);
        bind_partitioned_array_double(module);
    }

}  // namespace lue::framework
