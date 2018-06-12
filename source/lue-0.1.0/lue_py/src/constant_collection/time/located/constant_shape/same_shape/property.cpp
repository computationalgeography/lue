#include "lue/constant_collection/time/located/constant_shape/same_shape/property.hpp"
#include "lue/py/conversion.hpp"
#include "lue/py/numpy.hpp"
#include <pybind11/pybind11.h>
// #include <numpy/arrayobject.h>


namespace py = pybind11;
using namespace pybind11::literals;

DEFINE_INIT_NUMPY()


namespace lue {
namespace constant_collection {
namespace time {
namespace located {
namespace constant_shape {
namespace same_shape {

void init_property(
    py::module& module)
{

    init_numpy();

    py::class_<Property, constant_shape::Property>(
        module,
        "Property",
        "Property docstring...")

        // .def(
        //     "reserve",
        //     &Property::reserve,
        //     "reserve docstring...",
        //     py::return_value_policy::reference_internal)

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
            py::handle const& numpy_type_id_object) -> Property
        {

            int numpy_type_id = NPY_NOTYPE;
            {
                PyArray_Descr* dtype;
                if(!PyArray_DescrConverter(
                        numpy_type_id_object.ptr(), &dtype)) {
                    throw py::error_already_set();
                }
                numpy_type_id = dtype->type_num;
                Py_DECREF(dtype);
            }

            hid_t file_type_id, memory_type_id;
            std::tie(file_type_id, memory_type_id) =
                numpy_type_to_hdf5_types(numpy_type_id);

            return create_property(
                self, name,
                hdf5::Datatype(file_type_id), hdf5::Datatype(memory_type_id));
        },
        R"(
    Create new property for storing 0D (scalar) values

    - The property will be added to *property_set*
)",
        "property_set"_a,
        "name"_a,
        "dtype"_a,
        py::return_value_policy::move)

        ;

    module.def(
        "create_property",
        [](
            PropertySet& self,
            std::string const& name,
            py::handle const& numpy_type_id_object,
            py::tuple const& shape) -> Property
        {

            int numpy_type_id = NPY_NOTYPE;
            {
                PyArray_Descr* dtype;
                if(!PyArray_DescrConverter(
                        numpy_type_id_object.ptr(), &dtype)) {
                    throw py::error_already_set();
                }
                numpy_type_id = dtype->type_num;
                Py_DECREF(dtype);
            }

            hdf5::Shape value_shape(shape.size());

            for(size_t i = 0; i < shape.size(); ++i) {
                value_shape[i] = py::int_(shape[i]);
            }

            hid_t file_type_id, memory_type_id;
            std::tie(file_type_id, memory_type_id) =
                numpy_type_to_hdf5_types(numpy_type_id);

            return create_property(
                self, name,
                hdf5::Datatype(file_type_id), hdf5::Datatype(memory_type_id),
                value_shape);
        },
        R"(
    Create new property for storing nD values

    - The rank of the values is determined by the size of the *value_shape*
      argument
    - The shape of the values is determined by the contents of the
      *value_shape* argument
    - The property will be added to the property set
)",
        "property_set"_a,
        "name"_a,
        "dtype"_a,
        "value_shape"_a,
        py::return_value_policy::move)

        ;

}

}  // namespace same_shape
}  // namespace constant_shape
}  // namespace located
}  // namespace time
}  // namespace constant_collection
}  // namespace lue
