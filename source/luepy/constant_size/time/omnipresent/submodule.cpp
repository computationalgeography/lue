#include <pybind11/pybind11.h>


namespace py = pybind11;


namespace lue {
namespace constant_size {
namespace time {
namespace omnipresent {
namespace different_shape {

void init_submodule(py::module& module);

}  // namespace different_shape


namespace same_shape {

void init_submodule(py::module& module);

}  // namespace same_shape


// void init_domain_class(py::module& module);
void init_property_class(py::module& module);
void init_property_set_class(py::module& module);
// void init_space_box_class(py::module& module);


void init_submodule(
    py::module& module)
{
    py::module submodule = module.def_submodule("omnipresent");

    // init_domain_class(submodule);
    init_property_class(submodule);
    init_property_set_class(submodule);

    different_shape::init_submodule(submodule);
    same_shape::init_submodule(submodule);

    // // Space box inherits same_shape::Item, so init the SpaceBox class after
    // // the same_shape sub module is initialized.
    // init_space_box_class(submodule);
}

} // namespace omnipresent
} // namespace time
} // namespace constant_size
} // namespace lue
