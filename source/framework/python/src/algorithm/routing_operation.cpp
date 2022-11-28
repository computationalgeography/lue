#include <pybind11/pybind11.h>


namespace lue::framework {

    void bind_kinematic_wave(pybind11::module& module);


    void bind_routing_operations(
        pybind11::module& module)
    {
        bind_kinematic_wave(module);
    }

}  // namespace lue::framework
