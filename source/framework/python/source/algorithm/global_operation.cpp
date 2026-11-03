#include <pybind11/pybind11.h>


namespace lue::framework {

    void bind_all(pybind11::module& module);
    void bind_any(pybind11::module& module);
    void bind_global_operation(pybind11::module& module);
    void bind_none(pybind11::module& module);
    void bind_reclassify(pybind11::module& module);


    PYBIND11_EXPORT void bind_global_operations(pybind11::module& module)
    {
        bind_all(module);
        bind_any(module);
        bind_global_operation(module);
        bind_none(module);
        bind_reclassify(module);
    }

}  // namespace lue::framework
