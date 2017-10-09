#include "lue/constant_size/time/omnipresent/space_box_domain.hpp"
#include "lue/py/conversion.hpp"
#include "lue/py/numpy.hpp"
#include <pybind11/pybind11.h>
#include <numpy/arrayobject.h>


namespace py = pybind11;
using namespace pybind11::literals;

DEFINE_INIT_NUMPY()


namespace lue {
namespace constant_size {
namespace time {
namespace omnipresent {

void init_space_box_domain_class(
    py::module& module)
{

    init_numpy();

    py::class_<SpaceBoxDomain, SpaceDomain>(
        module,
        "SpaceBoxDomain",
        "SpaceBoxDomain docstring...")

        .def(
            "reserve",
            &SpaceBoxDomain::reserve,
            "reserve docstring...",
            py::return_value_policy::reference_internal)

        .def_property_readonly(
            "items",
            py::overload_cast<>(&SpaceBoxDomain::items),
            "items docstring...",
            py::return_value_policy::reference_internal)

        ;


    module.def(
        "create_space_box_domain",
        [](
            PropertySet& property_set,
            py::handle const& numpy_type_id_object,
            size_t const rank) // -> SpaceBoxDomain
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

            return create_space_box_domain(property_set,
                hdf5::Datatype(file_type_id), hdf5::Datatype(memory_type_id),
                rank);
        },
        R"(
    Create new space box domain

    The domain will be added to the domain of the property set
)",
        "property_set"_a,
        "dtype"_a,
        "rank"_a,
        py::return_value_policy::move)

        ;

}

}  // namespace omnipresent
}  // namespace time
}  // namespace constant_size
}  // namespace lue
