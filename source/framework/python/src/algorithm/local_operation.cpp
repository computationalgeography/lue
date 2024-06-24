#include <pybind11/pybind11.h>


// TODO These are not all local operations. Split them over multiple
//     modules, or rename this module.
namespace lue::framework {

    void bind_abs(pybind11::module& module);
    void bind_accu(pybind11::module& module);
    void bind_accu3(pybind11::module& module);
    void bind_accu_fraction(pybind11::module& module);
    void bind_accu_info(pybind11::module& module);
    void bind_accu_info3(pybind11::module& module);
    void bind_accu_threshold(pybind11::module& module);
    void bind_accu_threshold3(pybind11::module& module);
    void bind_acos(pybind11::module& module);
    void bind_add(pybind11::module& module);
    void bind_array_partition_id(pybind11::module& module);
    void bind_all(pybind11::module& module);
    void bind_array_partition_id(pybind11::module& module);
    void bind_aspect(pybind11::module& module);
    void bind_asin(pybind11::module& module);
    void bind_atan(pybind11::module& module);
    void bind_atan2(pybind11::module& module);
    void bind_cast(pybind11::module& module);
    void bind_cos(pybind11::module& module);
    void bind_ceil(pybind11::module& module);
    void bind_cell_index(pybind11::module& module);
    void bind_d8_flow_direction(pybind11::module& module);
    void bind_divide(pybind11::module& module);
    void bind_downstream(pybind11::module& module);
    void bind_downstream_distance(pybind11::module& module);
    void bind_equal_to(pybind11::module& module);
    void bind_exp(pybind11::module& module);
    void bind_floor(pybind11::module& module);
    void bind_greater_than(pybind11::module& module);
    void bind_greater_than_equal_to(pybind11::module& module);
    void bind_inflow_count(pybind11::module& module);
    void bind_inflow_count3(pybind11::module& module);
    void bind_inter_partition_stream(pybind11::module& module);
    void bind_iterate_per_element(pybind11::module& module);
    void bind_less_than(pybind11::module& module);
    void bind_less_than_equal_to(pybind11::module& module);
    void bind_locality_id(pybind11::module& module);
    void bind_log(pybind11::module& module);
    void bind_log10(pybind11::module& module);
    void bind_logical_and(pybind11::module& module);
    void bind_logical_exclusive_or(pybind11::module& module);
    void bind_logical_inclusive_or(pybind11::module& module);
    void bind_logical_not(pybind11::module& module);
    void bind_multiply(pybind11::module& module);
    void bind_negate(pybind11::module& module);
    void bind_normal(pybind11::module& module);
    void bind_not_equal_to(pybind11::module& module);
    void bind_pow(pybind11::module& module);
    void bind_round(pybind11::module& module);
    void bind_sin(pybind11::module& module);
    void bind_slope(pybind11::module& module);
    void bind_sqrt(pybind11::module& module);
    void bind_subtract(pybind11::module& module);
    void bind_tan(pybind11::module& module);
    void bind_timestamp(pybind11::module& module);
    void bind_uniform(pybind11::module& module);
    void bind_unique_id(pybind11::module& module);
    void bind_upstream(pybind11::module& module);
    void bind_valid(pybind11::module& module);
    void bind_where(pybind11::module& module);


    void bind_local_operations(pybind11::module& module)
    {
        bind_abs(module);
        bind_accu(module);
        bind_accu3(module);
        bind_accu_fraction(module);
        bind_accu_info(module);
        bind_accu_info3(module);
        bind_accu_threshold(module);
        bind_accu_threshold3(module);
        bind_acos(module);
        bind_add(module);
        bind_all(module);
        bind_array_partition_id(module);
        bind_aspect(module);
        bind_asin(module);
        bind_atan(module);
        bind_atan2(module);
        bind_cast(module);
        bind_ceil(module);
        bind_cell_index(module);
        bind_cos(module);
        bind_d8_flow_direction(module);
        bind_divide(module);
        bind_downstream(module);
        bind_downstream_distance(module);
        bind_equal_to(module);
        bind_exp(module);
        bind_floor(module);
        bind_greater_than(module);
        bind_greater_than_equal_to(module);
        bind_inflow_count(module);
        bind_inflow_count3(module);
        bind_inter_partition_stream(module);
        bind_iterate_per_element(module);
        bind_less_than(module);
        bind_less_than_equal_to(module);
        bind_locality_id(module);
        bind_log(module);
        bind_log10(module);
        bind_logical_and(module);
        bind_logical_exclusive_or(module);
        bind_logical_inclusive_or(module);
        bind_logical_not(module);
        bind_multiply(module);
        bind_negate(module);
        bind_normal(module);
        bind_not_equal_to(module);
        bind_pow(module);
        bind_round(module);
        bind_sin(module);
        bind_slope(module);
        bind_sqrt(module);
        bind_subtract(module);
        bind_tan(module);
        bind_timestamp(module);
        bind_uniform(module);
        bind_unique_id(module);
        bind_upstream(module);
        bind_valid(module);
        bind_where(module);
    }

}  // namespace lue::framework
