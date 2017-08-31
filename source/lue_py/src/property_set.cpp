#include "collection.hpp"
#include "lue/property_sets.hpp"
#include "lue/constant_size/time/omnipresent/property_set.hpp"
#include <pybind11/pybind11.h>
// #include <iostream>
// #include "lue/python_api/numpy.h"
// #include <pybind11/numpy.h>
// #include <numpy/arrayobject.h>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {

// DEFINE_INIT_NUMPY()


void init_property_set_class(
    py::module& module)
{

    // init_numpy();

    BASE_COLLECTION(PropertySets, PropertySet)


    py::class_<PropertySets, PropertySetCollection>(
        module,
        "PropertySets",
        R"(
    PropertySets docstring...
)")

        .def(
            "__getitem__",
            [](
                PropertySets& self,
                std::string const& name)
            {
                auto& property_set = self[name];
                auto const& configuration = property_set.configuration();

                // TODO
                //     Support registering of casters by specialized
                //     PropertySet classes.
                py::object object;

                switch(configuration.size_of_item_collection_type()) {
                    case SizeOfItemCollectionType::constant_size: {

                        // TODO Switch on time domain time.


                        object = py::cast(new
                                constant_size::time::omnipresent::PropertySet(
                            property_set.id()));
                        break;
                    }
                }

                return object;
            },
    "Return item\n"
    "\n"
    ":param str name: Name of item to find\n"
    ":raises RuntimeError: In case the collection does not contain the\n"
    "   item\n",
            "name"_a
        )

        // .def("__repr__",
        //     [](PropertySets const& property_sets) {
        //         return "PropertySets(size=" + std::to_string(
        //             property_sets.size()) + ")";
        //     }
        // )
        // .def("add", &PropertySets::add,
        //     "add docstring...",
        //     py::return_value_policy::reference_internal)

        ;


    // py::enum_<SizeOfItemCollectionType>(module, "size_of_item_collection",
    //     "size_of_item_collection docstring...")
    //     .value("constant", SizeOfItemCollectionType::constant_size)
    // ;


    // py::class_<PropertySetConfiguration>(module, "PropertySetConfiguration",
    //     "PropertySetConfiguration docstring...")
    //     .def(py::init<SizeOfItemCollectionType const>(),
    //         "__init__ docstring...",
    //         "type"_a)
    //     .def_property_readonly("size_of_item_collection_type",
    //             &PropertySetConfiguration::size_of_item_collection_type,
    //         "size_of_item_collection_type docstring...")
    // ;


    // py::class_<PropertySet::Configuration>(
    //     module,
    //     "PropertySetConfiguration",
    //     "PropertySetConfiguration docstring...")

    //     .def_property_readonly(
    //         "size_of_item_collection_type",
    //         &PropertySet::Configuration::size_of_item_collection_type,
    //         "size_of_item_collection_type docstring..."
    //     )

    //     ;


    py::class_<PropertySet, hdf5::Group>(
        module,
        "PropertySet",
        "PropertySet docstring...")

        .def("__repr__",
            [](PropertySet const& property_set) {
                return "PropertySet(pathname='" +
                    property_set.id().pathname() + "')";
            }
        )

        .def_property_readonly(
            "name",
            [](PropertySet const& self)
            {
                return self.id().name();
            },
            "name docstring...")


    //     .def_property_readonly("configuration", &PropertySet::configuration,
    //         "configuration docstring...",
    //         py::return_value_policy::reference_internal)
    //     .def_property_readonly("domain", &PropertySet::domain,
    //         "domain docstring...",
    //         py::return_value_policy::reference_internal)
    //     // .def("add_property", &PropertySet::add_property,
    //     //     "add_property docstring...",
    //     //     py::return_value_policy::reference_internal)

//         .def_property_readonly(
//             "properties",
//             py::overload_cast<>(&PropertySet::properties),
//             R"(
//     Return properties collection
// 
//     :rtype: lue.Properties
// )",
//             py::return_value_policy::reference_internal)

        ;


    /*
#define cast(object, type) \
    try {  \
        object.cast<type&>();  \
        std::cout << " " #type "&\n";  \
    }  \
    catch(...) {  \
        std::cout << "!" #type "&\n";  \
    }  \
    try {  \
        object.cast<type*>();  \
        std::cout << " " #type "*\n";  \
    }  \
    catch(...) {  \
        std::cout << "!" #type "*\n";  \
    }  \
    try {  \
        object.cast<type const&>();  \
        std::cout << " " #type " const&\n";  \
    }  \
    catch(...) {  \
        std::cout << "!" #type " const&\n";  \
    }  \
    try {  \
        object.cast<type const*>();  \
        std::cout << " " #type " const*\n";  \
    }  \
    catch(...) {  \
        std::cout << "!" #type " const*\n";  \
    }
    */


    // py::class_<time::omnipresent::size_per_item::constant::PropertySet>(
    //     module, "O_C_PropertySet",
    //     py::base<time::omnipresent::PropertySet>(),
    //     "O_C_PropertySet docstring...")

    //     .def(py::init<PropertySet&>(),
    //         "__init__ docstring..."
    //         "group"_a,
    //         py::keep_alive<1, 2>())
    // //     .def("add_property", [](
    // //                 time::omnipresent::omnipresent::PropertySet& self,
    // //                 std::string const& name,
    // //                 py::handle const& numpy_type_id_object,
    // //                 py::tuple const& shape,
    // //                 py::tuple const& chunks) ->
    // //                     time::omnipresent::omnipresent::Property& {

    // //             int numpy_type_id = NPY_NOTYPE;
    // //             {
    // //                 PyArray_Descr* dtype;
    // //                 if(!PyArray_DescrConverter(numpy_type_id_object.ptr(),
    // //                         &dtype)) {
    // //                     throw py::error_already_set();
    // //                 }
    // //                 numpy_type_id = dtype->type_num;
    // //                 Py_DECREF(dtype);
    // //             }

    // //             Shape shape_(shape.size());

    // //             for(size_t i = 0; i < shape.size(); ++i) {
    // //                 shape_[i] = py::int_(shape[i]);
    // //             }

    // //             Chunks chunks_(chunks.size());

    // //             for(size_t i = 0; i < chunks.size(); ++i) {
    // //                 chunks[i] = chunks[i];
    // //             }

    // //             return self.add_property(name,
    // //                 numpy_type_to_hdf5_type(numpy_type_id), shape_, chunks_);
    // //         },
    // //         // &time::omnipresent::omnipresent::PropertySet::add_property,
    // //         "add_property docstring...",
    // //         py::return_value_policy::reference_internal)
    // //     .def("reserve_items",
    // //             &time::omnipresent::omnipresent::PropertySet::reserve_items,
    // //         "reserve docstring...",
    // //         py::return_value_policy::reference_internal)
    // //     .def_property_readonly("items",
    // //             &time::omnipresent::omnipresent::PropertySet::items,
    // //         "items docstring...",
    // //         py::return_value_policy::reference_internal)
    // ;

}

}  // namespace lue
