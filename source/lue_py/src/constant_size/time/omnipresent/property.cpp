#include "lue/py/constant_size/time/omnipresent/property.hpp"
#include "lue/constant_size/time/omnipresent/different_shape/property.hpp"
#include "lue/constant_size/time/omnipresent/same_shape/property.hpp"


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {
namespace constant_size {
namespace time {
namespace omnipresent {

py::object cast_to_specialized_property(
    Property const& property)
{

    auto& configuration = property.configuration();

    // TODO
    //     Support registering of casters by specialized
    //     PropertySet classes.
    py::object object;

    switch(configuration.shape_per_item()) {
        case ShapePerItem::same: {
            auto file_datatype =
                same_shape::Property::file_datatype(property);
            object = py::cast(new same_shape::Property(
                hdf5::Group{property.id()}, memory_datatype(file_datatype)));
            break;
        }
        case ShapePerItem::different: {
            auto file_datatype =
                different_shape::Property::file_datatype(property);
            object = py::cast(new different_shape::Property(
                hdf5::Group{property.id()}, memory_datatype(file_datatype)));
            break;
        }
    }

    return object;
}


void init_property_class(
    py::module& module)
{

    py::class_<Property, constant_size::Property>(
        module,
        "Property",
        "Property docstring...")

        .def_property_readonly(
            "space_discretization",
            [](
                Property& self)
            {
                return cast_to_specialized_property(
                    omnipresent::Property{self.space_discretization()});
            },
            "space_discretization docstring...")

        ;


    // TODO This must be a member of a Properties class.


//         .def(
//             "__getitem__",
//             [](
//                 Properties& self,
//                 std::string const& name)
//             {
//                 auto& property = self[name];
//                 auto const& configuration = property.configuration();
// 
//                 // TODO
//                 //     Support registering of casters by specialized
//                 //     Property classes.
//                 py::object object;
// 
//                 switch(configuration.shape_per_item_type()) {
//                     case ShapePerItem::same: {
//                         object = py::cast(new
//                                 constant_size::time::omnipresent::
//                                     same_shape::Property(
//                             property.id()));
//                         break;
//                     }
//                 }
// 
//                 return object;
//             },
//     "Return item\n"
//     "\n"
//     ":param str name: Name of item to find\n"
//     ":raises RuntimeError: In case the collection does not contain the\n"
//     "   item\n",
//             "name"_a
//         )


}

}  // namespace omnipresent
}  // namespace time
}  // namespace constant_size
}  // namespace lue
