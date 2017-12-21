#include "lue/constant_size/time/located/shared/constant_shape/different_shape/property.hpp"
#include "lue/py/conversion.hpp"
#include "lue/py/numpy.hpp"
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <numpy/arrayobject.h>


namespace py = pybind11;
using namespace pybind11::literals;

DEFINE_INIT_NUMPY()


namespace lue {
namespace constant_size {
namespace time {
namespace located {
namespace shared {
namespace constant_shape {
namespace different_shape {

void init_property(
    py::module& module)
{

    init_numpy();

    py::class_<Property, constant_shape::Property>(
        module,
        "Property",
        "Property docstring...")

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
}  // namespace constant_shape
}  // namespace shared
}  // namespace located
}  // namespace time
}  // namespace constant_size
}  // namespace lue
