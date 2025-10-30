#include "lue/configure.hpp"
#include <pybind11/pybind11.h>


namespace lue::framework {

    void bind_abs(pybind11::module& module);
    void bind_all(pybind11::module& module);
    void bind_arithmetic(pybind11::module& module);
    void bind_cast(pybind11::module& module);
    void bind_cell_index(pybind11::module& module);
    void bind_comparison(pybind11::module& module);
#ifdef LUE_FRAMEWORK_WITH_DEVELOPMENT_OPERATIONS
    void bind_iterate_per_element(pybind11::module& module);
#endif
    void bind_logical(pybind11::module& module);
    void bind_modulus(pybind11::module& module);
    void bind_nearest_integer(pybind11::module& module);
    void bind_trigonometry(pybind11::module& module);
    void bind_unique_id(pybind11::module& module);
    void bind_valid(pybind11::module& module);
    void bind_where(pybind11::module& module);


    PYBIND11_EXPORT void bind_local_operations(pybind11::module& module)
    {
        bind_abs(module);
        bind_all(module);
        bind_arithmetic(module);
        bind_cast(module);
        bind_cell_index(module);
        bind_comparison(module);
#ifdef LUE_FRAMEWORK_WITH_DEVELOPMENT_OPERATIONS
        bind_iterate_per_element(module);
#endif
        bind_logical(module);
        bind_modulus(module);
        bind_nearest_integer(module);
        bind_trigonometry(module);
        bind_unique_id(module);
        bind_valid(module);
        bind_where(module);
    }

}  // namespace lue::framework
