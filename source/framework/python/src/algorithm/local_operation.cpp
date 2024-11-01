#include <pybind11/pybind11.h>


namespace lue::framework {

    void bind_abs(pybind11::module& module);
    void bind_add(pybind11::module& module);
    void bind_all(pybind11::module& module);
    void bind_cast(pybind11::module& module);
    void bind_ceil(pybind11::module& module);
    void bind_cell_index(pybind11::module& module);
    void bind_divide(pybind11::module& module);
    void bind_equal_to(pybind11::module& module);
    void bind_exp(pybind11::module& module);
    void bind_floor(pybind11::module& module);
    void bind_greater_than(pybind11::module& module);
    void bind_greater_than_equal_to(pybind11::module& module);
    void bind_iterate_per_element(pybind11::module& module);
    void bind_less_than(pybind11::module& module);
    void bind_less_than_equal_to(pybind11::module& module);
    void bind_log(pybind11::module& module);
    void bind_log10(pybind11::module& module);
    void bind_logical(pybind11::module& module);
    void bind_multiply(pybind11::module& module);
    void bind_negate(pybind11::module& module);
    void bind_not_equal_to(pybind11::module& module);
    void bind_pow(pybind11::module& module);
    void bind_round(pybind11::module& module);
    void bind_sqrt(pybind11::module& module);
    void bind_subtract(pybind11::module& module);
    void bind_trigonometry(pybind11::module& module);
    void bind_unique_id(pybind11::module& module);
    void bind_valid(pybind11::module& module);
    void bind_where(pybind11::module& module);


    PYBIND11_EXPORT void bind_local_operations(pybind11::module& module)
    {
        bind_abs(module);
        bind_trigonometry(module);
        // bind_add(module);
        // bind_all(module);
        // bind_cast(module);
        // bind_ceil(module);
        // bind_cell_index(module);
        // bind_divide(module);
        // bind_equal_to(module);
        // bind_exp(module);
        // bind_floor(module);
        // bind_greater_than(module);
        // bind_greater_than_equal_to(module);
        // bind_iterate_per_element(module);
        // bind_less_than(module);
        // bind_less_than_equal_to(module);
        // bind_log(module);
        // bind_log10(module);
        bind_logical(module);
        // bind_multiply(module);
        // bind_negate(module);
        // bind_not_equal_to(module);
        // bind_pow(module);
        // bind_round(module);
        // bind_sqrt(module);
        // bind_subtract(module);
        // bind_unique_id(module);
        // bind_valid(module);
        // bind_where(module);
    }

}  // namespace lue::framework
