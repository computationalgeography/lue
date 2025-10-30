#include "lue/configure.hpp"
#include <pybind11/pybind11.h>


namespace lue::framework {

    void bind_accu(pybind11::module& module);
    void bind_accu_fraction(pybind11::module& module);
#ifdef LUE_FRAMEWORK_WITH_DEVELOPMENT_OPERATIONS
    void bind_accu_info(pybind11::module& module);
#endif
    void bind_accu_threshold(pybind11::module& module);
    void bind_d8_flow_direction(pybind11::module& module);
    void bind_decreasing_order(pybind11::module& module);
    void bind_downstream(pybind11::module& module);
    void bind_downstream_distance(pybind11::module& module);
    void bind_first_n(pybind11::module& module);
#ifdef LUE_FRAMEWORK_WITH_DEVELOPMENT_OPERATIONS
    void bind_inflow_count(pybind11::module& module);
#endif
    void bind_kinematic_wave(pybind11::module& module);
#ifdef LUE_FRAMEWORK_WITH_DEVELOPMENT_OPERATIONS
    void bind_partial_accu(pybind11::module& module);
#endif
    void bind_upstream(pybind11::module& module);


    PYBIND11_EXPORT void bind_routing_operations(pybind11::module& module)
    {
        bind_accu(module);
        bind_accu_fraction(module);
#ifdef LUE_FRAMEWORK_WITH_DEVELOPMENT_OPERATIONS
        bind_accu_info(module);
#endif
        bind_accu_threshold(module);
        bind_d8_flow_direction(module);
        bind_decreasing_order(module);
        bind_downstream(module);
        bind_downstream_distance(module);
        bind_first_n(module);
#ifdef LUE_FRAMEWORK_WITH_DEVELOPMENT_OPERATIONS
        bind_inflow_count(module);
#endif
        bind_kinematic_wave(module);
#ifdef LUE_FRAMEWORK_WITH_DEVELOPMENT_OPERATIONS
        bind_partial_accu(module);
#endif
        bind_upstream(module);
    }

}  // namespace lue::framework
