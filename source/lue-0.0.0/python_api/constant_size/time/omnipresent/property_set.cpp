#include "lue/cxx_api/constant_size/time/omnipresent/property_set.h"
// #include "lue/cxx_api/constant_size.h"
// #include "lue/cxx_api/property_sets.h"
#include "lue/python_api/numpy.h"
#include <pybind11/pybind11.h>
#include <numpy/arrayobject.h>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {
namespace python {
namespace constant_size {
namespace time {
namespace omnipresent {

DEFINE_INIT_NUMPY()


void init_property_set_class(
    py::module& module)
{

    init_numpy();

    py::class_<
            lue::constant_size::time::omnipresent::PropertySet,
            lue::constant_size::PropertySet>(
                module,
                "PropertySet",
                "PropertySet docstring...")

        .def(
            py::init<PropertySet&>(),
            "__init__ docstring..."
            "group"_a,
            py::keep_alive<1, 2>())

        .def(
            "reserve_items",
            &lue::constant_size::time::omnipresent::PropertySet::reserve_items,
            "reserve_items docstring...",
            py::return_value_policy::reference_internal)

        // .def_property_readonly("items",
        //         &time::omnipresent::PropertySet::items,
        //     "items docstring...",
        //     py::return_value_policy::reference_internal)

        .def_property_readonly(
            "ids",
            &lue::constant_size::time::omnipresent::PropertySet::ids,
            "ids docstring...",
            py::return_value_policy::reference_internal)

        .def(
            "add_property",
            [](
                lue::constant_size::time::omnipresent::PropertySet& self,
                std::string const& name,
                py::handle const& numpy_type_id_object,
                py::tuple const& shape,
                py::tuple const& chunks) ->
                    lue::constant_size::time::omnipresent::same_shape::Property&
            {

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

                hid_t file_type_id, memory_type_id;
                std::tie(file_type_id, memory_type_id) =
                    numpy_type_to_hdf5_types(numpy_type_id);

                return self.add_property(name, file_type_id, memory_type_id,
                    shape_, chunks_);
            },
            "add_property docstring...",
            py::return_value_policy::reference_internal)

        .def(
            "add_property", [](
                    lue::constant_size::time::omnipresent::PropertySet& self,
                    std::string const& name,
                    py::handle const& numpy_type_id_object,
                    size_t const rank) ->
                        lue::constant_size::time::omnipresent::different_shape::Property& {

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

                hid_t file_type_id, memory_type_id;
                std::tie(file_type_id, memory_type_id) =
                    numpy_type_to_hdf5_types(numpy_type_id);

                return self.add_property(name, file_type_id, // memory_type_id,
                    rank);
            },
            "add_property docstring...",
            py::return_value_policy::reference_internal)
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
//     // //     .def("reserve_items",
//     // //             &time::omnipresent::omnipresent::PropertySet::reserve_items,
//     // //         "reserve docstring...",
//     // //         py::return_value_policy::reference_internal)
//     // //     .def_property_readonly("items",
//     // //             &time::omnipresent::omnipresent::PropertySet::items,
//     // //         "items docstring...",
//     // //         py::return_value_policy::reference_internal)
//     // ;
// 
}

}  // namespace omnipresent
}  // namespace time
}  // namespace constant_size
}  // namespace python
}  // namespace lue
