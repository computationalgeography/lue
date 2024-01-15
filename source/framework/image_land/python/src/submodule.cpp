#include "lue/py/image_land/submodule.hpp"


namespace lue::image_land {

    void bind_integrate(pybind11::module& module);
    void bind_integrate_and_allocate(pybind11::module& module);


    void init_submodule(pybind11::module& module)
    {
        pybind11::module submodule = module.def_submodule(
            "image_land",
            R"(
    :mod:`lue.image_land` --- IMAGE-LAND functionality
    ==================================================

    The :mod:`lue.image_land` package ...
)");

        bind_integrate(submodule);
        bind_integrate_and_allocate(submodule);
    }

}  // namespace lue::image_land
