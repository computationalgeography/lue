#include "lue/object/space/space_domain.hpp"
#include <pybind11/pybind11.h>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {

void init_space_domain(
    py::module& module)
{

    py::enum_<Mobility>(
        module,
        "mobility",
        "mobility docstring...")
        .value("mobile", Mobility::mobile)
        .value("stationary", Mobility::stationary)
        ;

    py::enum_<SpaceDomainItemType>(
        module,
        "space_domain_item_type",
        "space_domain_item_type docstring...")
        .value("box", SpaceDomainItemType::box)
        .value("point", SpaceDomainItemType::point)
        ;


    py::class_<SpaceConfiguration>(
        module,
        "SpaceConfiguration",
        R"(
    TODO
)")

        .def(
            py::init<Mobility, SpaceDomainItemType>())

        ;

}

}  // namespace lue
