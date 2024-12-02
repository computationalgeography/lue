#include <pybind11/pybind11.h>


namespace lue::framework {

    void bind_focal1(pybind11::module& module);
    void bind_focal2(pybind11::module& module);


    PYBIND11_EXPORT void bind_focal_operations(pybind11::module& module)
    {
        bind_focal1(module);
        bind_focal2(module);
    }

}  // namespace lue::framework
