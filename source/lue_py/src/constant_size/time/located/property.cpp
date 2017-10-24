#include "lue/constant_size/time/located/property.hpp"
#include <pybind11/pybind11.h>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {
namespace constant_size {
namespace time {
namespace located {

// py::object cast_to_specialized_property(
//     Property const& property)
// {
// 
//     py::object object;
// 
//     // auto& configuration = property.configuration();
// 
//     // switch(configuration.shape_per_item_type()) {
//     //     case ShapePerItemType::same: {
//     //         auto file_datatype =
//     //             same_shape::Property::file_datatype(property.id());
//     //         object = py::cast(new same_shape::Property(
//     //             property, memory_datatype(file_datatype)));
//     //         break;
//     //     }
//     //     case ShapePerItemType::different: {
//     //         auto file_datatype =
//     //             different_shape::Property::file_datatype(
//     //                 property.id());
//     //         object = py::cast(new different_shape::Property(
//     //             property, memory_datatype(file_datatype)));
//     //         break;
//     //     }
//     // }
// 
//     return object;
// }


void init_property(
    py::module& module)
{

    py::class_<Property, constant_size::Property>(
        module,
        "Property",
        "Property docstring...")

        .def("discretize_time",
            &Property::discretize_time,
            "discretize_time docstring...",
            "property"_a)

        .def_property_readonly(
            "time_is_discretized",
            &Property::time_is_discretized,
            "time_is_discretized docstring...")

        ;

}

}  // namespace located
}  // namespace time
}  // namespace constant_size
}  // namespace lue
