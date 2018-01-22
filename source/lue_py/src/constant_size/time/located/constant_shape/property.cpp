#include "lue/py/constant_size/time/omnipresent/property.hpp"
// #include "lue/constant_size/time/located/constant_shape/property.hpp"
// #include "lue/constant_size/time/located/constant_shape/different_shape/property.hpp"
#include "lue/constant_size/time/located/constant_shape/same_shape/property.hpp"
#include <pybind11/pybind11.h>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {
namespace constant_size {
namespace time {
namespace located {
namespace constant_shape {

// py::object cast_to_specialized_property(
//     Property const& property)
// {
// 
//     auto& configuration = property.configuration();
// 
//     // TODO
//     //     Support registering of casters by specialized
//     //     PropertySet classes.
//     py::object object;
// 
//     // switch(configuration.shape_per_item_type()) {
//     //     case ShapePerItemType::same: {
//     //         auto file_datatype =
//     //             same_shape::Property::file_datatype(
//     //                 property.id());
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

    py::class_<Property, located::Property>(
        module,
        "Property",
        "Property docstring...")

        .def_property_readonly(
            "time_discretization",
            [](
                Property& self)
            {
                return omnipresent::cast_to_specialized_property(
                    omnipresent::Property{
                        hdf5::Group{self.time_discretization().id()}});
            },
            "time_discretization docstring...")

        ;

}

}  // namespace constant_shape
}  // namespace located
}  // namespace time
}  // namespace constant_size
}  // namespace lue
