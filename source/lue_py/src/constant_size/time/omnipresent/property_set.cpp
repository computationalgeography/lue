#include "lue/py/constant_size/time/omnipresent/property.hpp"
#include "lue/constant_size/time/omnipresent/property_set.hpp"
#include "lue/constant_size/time/omnipresent/different_shape/property.hpp"
#include "lue/constant_size/time/omnipresent/same_shape/property.hpp"
#include "lue/phenomenon.hpp"
// #include <pybind11/stl.h>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {
namespace constant_size {
namespace time {
namespace omnipresent {

void init_property_set_class(
    py::module& module)
{

    py::class_<PropertySet, constant_size::PropertySet>(
        module,
        "PropertySet",
        "PropertySet docstring...")

        // .def(
        //     // py::init<PropertySet&>(),
        //     py::init<Phenomenon&, std::string const&>(),
        //     "__init__ docstring..."
        //     "phenomenon"_a,
        //     "name"_a,
        //     py::keep_alive<1, 2>())

        .def_property_readonly(
            "domain",
            py::overload_cast<>(&PropertySet::domain),
            "domain docstring...",
            py::return_value_policy::reference_internal)

        .def(
            "__getitem__",
            [](
                PropertySet& self,
                std::string const& name)
            {
                return cast_to_specialized_property(
                    omnipresent::Property{
                        hdf5::Group{self.properties()[name].id()}});
            },
            "Return property\n"
            "\n"
            ":param str name: Name of property to find\n"
            ":raises RuntimeError: In case the collection does not contain the\n"
            "   property\n",
            "name"_a)


        // .def(
        //     "add_property",
        //     [](
        //         lue::constant_size::time::omnipresent::PropertySet& self,
        //         std::string const& name,
        //         py::handle const& numpy_type_id_object,
        //         py::tuple const& shape,
        //         py::tuple const& chunks) ->
        //             lue::constant_size::time::omnipresent::same_shape::Property&
        //     {

        //         int numpy_type_id = NPY_NOTYPE;
        //         {
        //             PyArray_Descr* dtype;
        //             if(!PyArray_DescrConverter(numpy_type_id_object.ptr(),
        //                     &dtype)) {
        //                 throw py::error_already_set();
        //             }
        //             numpy_type_id = dtype->type_num;
        //             Py_DECREF(dtype);
        //         }

        //         Shape shape_(shape.size());

        //         for(size_t i = 0; i < shape.size(); ++i) {
        //             shape_[i] = py::int_(shape[i]);
        //         }

        //         Chunks chunks_(chunks.size());

        //         for(size_t i = 0; i < chunks.size(); ++i) {
        //             chunks_[i] = py::int_(chunks[i]);
        //         }

        //         hid_t file_type_id, memory_type_id;
        //         std::tie(file_type_id, memory_type_id) =
        //             numpy_type_to_hdf5_types(numpy_type_id);

        //         return self.add_property(name, file_type_id, memory_type_id,
        //             shape_, chunks_);
        //     },
        //     "add_property docstring...",
        //     py::return_value_policy::reference_internal)

        // .def(
        //     "add_property", [](
        //             lue::constant_size::time::omnipresent::PropertySet& self,
        //             std::string const& name,
        //             py::handle const& numpy_type_id_object,
        //             size_t const rank) ->
        //                 lue::constant_size::time::omnipresent::different_shape::Property& {

        //         int numpy_type_id = NPY_NOTYPE;
        //         {
        //             PyArray_Descr* dtype;
        //             if(!PyArray_DescrConverter(numpy_type_id_object.ptr(),
        //                     &dtype)) {
        //                 throw py::error_already_set();
        //             }
        //             numpy_type_id = dtype->type_num;
        //             Py_DECREF(dtype);
        //         }

        //         hid_t file_type_id, memory_type_id;
        //         std::tie(file_type_id, memory_type_id) =
        //             numpy_type_to_hdf5_types(numpy_type_id);

        //         return self.add_property(name, file_type_id, // memory_type_id,
        //             rank);
        //     },
        //     "add_property docstring...",
        //     py::return_value_policy::reference_internal)

        ;


//     // py::class_<time::omnipresent::size_per_item::constant::PropertySet>(
//     //     module, "O_C_PropertySet",
//     //     py::base<time::omnipresent::PropertySet>(),
//     //     "O_C_PropertySet docstring...")
// 
//     //     .def(py::init<PropertySet&>(),
//     //         "__init__ docstring..."
//     //         "group"_a,
//     //         py::keep_alive<1, 2>())
//     // //     .def("add_property", [](
//     // //                 time::omnipresent::omnipresent::PropertySet& self,
//     // //                 std::string const& name,
//     // //                 py::handle const& numpy_type_id_object,
//     // //                 py::tuple const& shape,
//     // //                 py::tuple const& chunks) ->
//     // //                     time::omnipresent::omnipresent::Property& {
// 
//     // //             int numpy_type_id = NPY_NOTYPE;
//     // //             {
//     // //                 PyArray_Descr* dtype;
//     // //                 if(!PyArray_DescrConverter(numpy_type_id_object.ptr(),
//     // //                         &dtype)) {
//     // //                     throw py::error_already_set();
//     // //                 }
//     // //                 numpy_type_id = dtype->type_num;
//     // //                 Py_DECREF(dtype);
//     // //             }
// 
//     // //             Shape shape_(shape.size());
// 
//     // //             for(size_t i = 0; i < shape.size(); ++i) {
//     // //                 shape_[i] = py::int_(shape[i]);
//     // //             }
// 
//     // //             Chunks chunks_(chunks.size());
// 
//     // //             for(size_t i = 0; i < chunks.size(); ++i) {
//     // //                 chunks[i] = chunks[i];
//     // //             }
// 
//     // //             return self.add_property(name,
//     // //                 numpy_type_to_hdf5_type(numpy_type_id), shape_, chunks_);
//     // //         },
//     // //         // &time::omnipresent::omnipresent::PropertySet::add_property,
//     // //         "add_property docstring...",
//     // //         py::return_value_policy::reference_internal)
//     // //     .def("reserve",
//     // //             &time::omnipresent::omnipresent::PropertySet::reserve,
//     // //         "reserve docstring...",
//     // //         py::return_value_policy::reference_internal)
//     // //     .def_property_readonly("items",
//     // //             &time::omnipresent::omnipresent::PropertySet::items,
//     // //         "items docstring...",
//     // //         py::return_value_policy::reference_internal)
//     // ;
// 

    module.def(
        "create_property_set",
        // py::overload_cast<Phenomenon&, std::string const&>(
        //     &create_property_set),
        [](
            Phenomenon& phenomenon,
            std::string const& name)
        {
            return create_property_set(phenomenon.property_sets(), name);
        },
        R"(
    Create new property set

    The property set will be added to the phenomenon
)",
        "phenomenon"_a,
        "name"_a,
        py::return_value_policy::move)

        ;

    module.def(
        "create_property_set",
        // py::overload_cast<
        //     Phenomenon&, std::string const&, same_shape::Value const&>(
        //         &create_property_set),
        [](
            Phenomenon& phenomenon,
            std::string const& name,
            same_shape::Value const& ids)
        {
            return create_property_set(
                phenomenon.property_sets(), name, ids);
        },
        R"(
    Create new property set

    The property set will be added to the phenomenon
)",
        "phenomenon"_a,
        "name"_a,
        "ids"_a,
        py::return_value_policy::move)

        ;

}

}  // namespace omnipresent
}  // namespace time
}  // namespace constant_size
}  // namespace lue
