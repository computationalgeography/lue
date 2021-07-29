#include <pybind11/pybind11.h>


// TODO These are not all local operations. Split them over multiple
//     modules, or rename this module.
namespace lue::framework {

    void bind_accu(pybind11::module& module);
    void bind_accu3(pybind11::module& module);
    void bind_accu_fraction(pybind11::module& module);
    void bind_accu_info(pybind11::module& module);
    void bind_accu_info3(pybind11::module& module);
    void bind_accu_threshold(pybind11::module& module);
    void bind_accu_threshold3(pybind11::module& module);
    void bind_add(pybind11::module& module);
    void bind_array_partition_id(pybind11::module& module);
    void bind_all(pybind11::module& module);
    void bind_array_partition_id(pybind11::module& module);
    void bind_cast(pybind11::module& module);
    void bind_d8_flow_direction(pybind11::module& module);
    void bind_divide(pybind11::module& module);
    void bind_downstream(pybind11::module& module);
    void bind_downstream_distance(pybind11::module& module);
    void bind_equal_to(pybind11::module& module);
    void bind_greater_than(pybind11::module& module);
    void bind_greater_than_equal_to(pybind11::module& module);
    void bind_inflow_count(pybind11::module& module);
    void bind_inflow_count3(pybind11::module& module);
    void bind_inter_partition_stream(pybind11::module& module);
    void bind_less_than(pybind11::module& module);
    void bind_less_than_equal_to(pybind11::module& module);
    void bind_locality_id(pybind11::module& module);
    void bind_log(pybind11::module& module);
    void bind_maximum(pybind11::module& module);
    void bind_minimum(pybind11::module& module);
    void bind_multiply(pybind11::module& module);
    void bind_not_equal_to(pybind11::module& module);
    void bind_pow(pybind11::module& module);
    void bind_slope(pybind11::module& module);
    void bind_sqrt(pybind11::module& module);
    void bind_subtract(pybind11::module& module);
    void bind_timestamp(pybind11::module& module);
    void bind_uniform(pybind11::module& module);
    void bind_upstream(pybind11::module& module);
    void bind_valid(pybind11::module& module);
    void bind_where(pybind11::module& module);


    void bind_local_operations(
        pybind11::module& module)
    {
        bind_accu(module);
        bind_accu3(module);
        bind_accu_fraction(module);
        bind_accu_info(module);
        bind_accu_info3(module);
        bind_accu_threshold(module);
        bind_accu_threshold3(module);
        bind_add(module);
        bind_all(module);
        bind_array_partition_id(module);
        bind_cast(module);
        bind_d8_flow_direction(module);
        bind_divide(module);
        bind_downstream(module);
        bind_downstream_distance(module);
        bind_equal_to(module);
        bind_greater_than(module);
        bind_greater_than_equal_to(module);
        bind_inflow_count(module);
        bind_inflow_count3(module);
        bind_inter_partition_stream(module);
        bind_less_than(module);
        bind_less_than_equal_to(module);
        bind_locality_id(module);
        bind_log(module);
        bind_maximum(module);
        bind_minimum(module);
        bind_multiply(module);
        bind_not_equal_to(module);
        bind_pow(module);
        bind_slope(module);
        bind_sqrt(module);
        bind_subtract(module);
        bind_timestamp(module);
        bind_uniform(module);
        bind_upstream(module);
        bind_valid(module);
        bind_where(module);
    }

}  // namespace lue::framework
