#include "lue/constant_collection/time/omnipresent/different_shape/property.hpp"
#include "lue/py/conversion.hpp"
#include "lue/py/numpy.hpp"
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <numpy/arrayobject.h>


namespace py = pybind11;
using namespace pybind11::literals;

DEFINE_INIT_NUMPY()


namespace lue {
namespace constant_collection {
namespace time {
namespace omnipresent {
namespace different_shape {

void init_property_class(
    py::module& module)
{

    init_numpy();

    py::class_<Property, omnipresent::Property>(
        module,
        "Property",
        "Property docstring...")




        // .def(
        //     py::init<Property& /* , hid_t const */>(),
        //     "__init__ docstring..."
        //     "group"_a,
        //     // "type"_a,
        //     py::keep_alive<1, 2>())

        // // .def("reserve",
        // //         &time::omnipresent::different_shape::
        // //             Property::reserve,
        // //     "reserve docstring...",
        // //     py::return_value_policy::reference_internal)

//         .def(
//             "reserve",
//             [](
//                 Property& property,
//                 py::array_t<hsize_t, py::array::c_style>& shapes) -> Value&
//             {
//                 static_assert(sizeof(hsize_t) == sizeof(uint64_t), "");
// 
//                 // shapes must be an nD array where:
//                 // - the number of dimensions must equal rank + 1
//                 // - the first dimension corresponds with the nr_items
//                 // - subsequent dimensions correspond with the extents of
//                 //   each item's value
// 
//                 auto const array_info = shapes.request();
// 
//                 // if(static_cast<rank_t>(array_info.ndim) != 2) {
//                 if(array_info.ndim != 2) {
//                     throw std::runtime_error(
//                         "rank of shapes array (" +
//                         std::to_string(array_info.ndim) + ") must equal 2");
//                 }
// 
//                 if(static_cast<int>(array_info.shape[1]) !=
//                         property.values().rank()) {
//                     throw std::runtime_error(
//                         "extent of second dimension of shapes array (" +
//                         std::to_string(array_info.shape[1]) +
//                         ") must equal rank of values (" +
//                         std::to_string(property.values().rank()) + ")");
//                 }
// 
//                 hsize_t const nr_items = array_info.shape[0];
// 
//                 return property.reserve(nr_items,
//                     static_cast<hsize_t*>(array_info.ptr));
//             },
//             "reserve docstring...",
//             py::return_value_policy::reference_internal)

        .def_property_readonly(
            "values",
            py::overload_cast<>(&Property::values),
            "values docstring...",
            py::return_value_policy::reference_internal)

        ;

    module.def(
        "create_property",
        [](
            PropertySet& self,
            std::string const& name,
            py::handle const& numpy_type_id_object,
            int const rank) -> Property
        {

            int numpy_type_id = NPY_NOTYPE;
            {
                PyArray_Descr* dtype;
                if(!PyArray_DescrConverter(numpy_type_id_object.ptr(), &dtype)) {
                    throw py::error_already_set();
                }
                numpy_type_id = dtype->type_num;
                Py_DECREF(dtype);
            }

            // hdf5::Shape value_shape(shape.size());

            // for(size_t i = 0; i < shape.size(); ++i) {
            //     value_shape[i] = py::int_(shape[i]);
            // }

            // hdf5::Shape value_chunk(chunks.size());

            // for(size_t i = 0; i < chunks.size(); ++i) {
            //     value_chunk[i] = py::int_(chunks[i]);
            // }

            hid_t file_type_id, memory_type_id;
            std::tie(file_type_id, memory_type_id) =
                numpy_type_to_hdf5_types(numpy_type_id);

            return create_property(self, name, hdf5::Datatype(file_type_id),
                hdf5::Datatype(memory_type_id), rank);
        },
        R"(
    Create new property

    The property will be added to the property set
)",
        "property_set"_a,
        "name"_a,
        "dtype"_a,
        "rank"_a,
        py::return_value_policy::move)

        ;

}

}  // namespace different
}  // namespace omnipresent
}  // namespace time
}  // namespace constant_collection
}  // namespace lue
