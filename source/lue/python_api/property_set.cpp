#include "lue/cxx_api/property_sets.h"
#include "lue/cxx_api/data_type.h"
#include "lue/python_api/collection.h"
#include "lue/python_api/numpy.h"
#include <pybind11/numpy.h>
#include <numpy/arrayobject.h>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {
namespace python {

DEFINE_INIT_NUMPY()


hid_t numpy_type_to_hdf5_type(
    int type_id)
{
    hid_t result = -1;

    switch(type_id) {
        case NPY_UINT32: {
            result = H5T_NATIVE_UINT32;
            break;
        }
        case NPY_INT32: {
            result = H5T_NATIVE_INT32;
            break;
        }
        case NPY_UINT64: {
            result = H5T_NATIVE_UINT64;
            break;
        }
        case NPY_INT64: {
            result = H5T_NATIVE_INT64;
            break;
        }
        case NPY_FLOAT32: {
            result = H5T_NATIVE_FLOAT;
            break;
        }
        case NPY_FLOAT64: {
            result = H5T_NATIVE_DOUBLE;
            break;
        }
        default: {
            throw std::runtime_error("Unsupported numpy type");
            break;
        }
    }

    return result;
}


void init_property_set(
        py::module& module)
{

    init_numpy();

    BASE_COLLECTION(PropertySet)

    py::class_<PropertySets>(module, "PropertySets",
            py::base<PropertySetCollection>(),
        "PropertySets docstring...")
        .def("__repr__",
            [](PropertySets const& property_sets) {
                return "PropertySets(size=" + std::to_string(
                    property_sets.size()) + ")";
            }
        )
        // .def("add", &PropertySets::add,
        //     "add docstring...",
        //     py::return_value_policy::reference_internal)
    ;

    py::class_<PropertySet>(module, "PropertySet", py::base<hdf5::Group>(),
        "PropertySet docstring...")
        .def("__repr__",
            [](PropertySet const& property_set) {
                return "PropertySet(pathname='" +
                    property_set.id().pathname() + "')";
            }
        )
        .def_property_readonly("domain", &PropertySet::domain,
            "domain docstring...",
            py::return_value_policy::reference_internal)
        // .def("add_property", &PropertySet::add_property,
        //     "add_property docstring...",
        //     py::return_value_policy::reference_internal)
        .def_property_readonly("properties", &PropertySet::properties,
            "properties docstring...",
            py::return_value_policy::reference_internal)
    ;

    // TODO Refactor Group and PropertySet API's into common base classes.
    //      Multiple classes implement the interfaces.
    py::class_<time::PropertySet>(module, "_PropertySet",
        "_PropertySet docstring...")

        // Group API
        .def_property_readonly("id", &time::PropertySet::id,
            "id docstring...",
            py::return_value_policy::reference_internal)
        .def_property_readonly("domain", &time::PropertySet::domain,
            "domain docstring...",
            py::return_value_policy::reference_internal)

        // PropertySet API
        // .def("add_property", &time::PropertySet::add_property,
        //     "add_property docstring...",
        //     py::return_value_policy::reference_internal)
        .def_property_readonly("properties", &time::PropertySet::properties,
            "properties docstring...",
            py::return_value_policy::reference_internal)
    ;

    py::class_<time::omnipresent::PropertySet>(module, "O_PropertySet",
        py::base<time::PropertySet>(),
        "O_PropertySet docstring...")

        .def(py::init<PropertySet&>(),
            "__init__ docstring..."
            "group"_a,
            py::keep_alive<1, 2>())

        .def("reserve_items",
                &time::omnipresent::PropertySet::reserve_items,
            "reserve docstring...",
            py::return_value_policy::reference_internal)

        .def_property_readonly("items",
                &time::omnipresent::PropertySet::items,
            "items docstring...",
            py::return_value_policy::reference_internal)

        .def("add_property", [](
                    time::omnipresent::PropertySet& self,
                    std::string const& name,
                    py::handle const& numpy_type_id_object,
                    py::tuple const& shape,
                    py::tuple const& chunks) ->
                        time::omnipresent::constant_shape::Property& {

                int numpy_type_id = NPY_NOTYPE;
                {
                    PyArray_Descr* dtype;
                    if(!PyArray_DescrConverter(numpy_type_id_object.ptr(),
                            &dtype)) {
                        throw py::error_already_set();
                    }
                    numpy_type_id = dtype->type_num;
                    Py_DECREF(dtype);
                }

                Shape shape_(shape.size());

                for(size_t i = 0; i < shape.size(); ++i) {
                    shape_[i] = py::int_(shape[i]);
                }

                Chunks chunks_(chunks.size());

                for(size_t i = 0; i < chunks.size(); ++i) {
                    chunks_[i] = py::int_(chunks[i]);
                }

                return self.add_property(name,
                    numpy_type_to_hdf5_type(numpy_type_id), shape_, chunks_);
            },
            "add_property docstring...",
            py::return_value_policy::reference_internal)

        .def("add_property", [](
                    time::omnipresent::PropertySet& self,
                    std::string const& name,
                    py::handle const& numpy_type_id_object) ->
                        time::omnipresent::variable_shape::Property& {

                int numpy_type_id = NPY_NOTYPE;
                {
                    PyArray_Descr* dtype;
                    if(!PyArray_DescrConverter(numpy_type_id_object.ptr(),
                            &dtype)) {
                        throw py::error_already_set();
                    }
                    numpy_type_id = dtype->type_num;
                    Py_DECREF(dtype);
                }

                return self.add_property(name,
                    numpy_type_to_hdf5_type(numpy_type_id));
            },
            "add_property docstring...",
            py::return_value_policy::reference_internal)
    ;


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

}  // namespace python
}  // namespace lue
