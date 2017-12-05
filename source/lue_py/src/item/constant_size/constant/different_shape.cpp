#include "lue/item/constant_size/constant/different_shape.hpp"
#include "lue/py/conversion.hpp"
#include <pybind11/pybind11.h>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {
namespace constant_size {
namespace constant {

void init_different_shape(
    py::module& module)
{

    py::class_<DifferentShape, hdf5::Group>(
        module,
        "DifferentShape",
        "DifferentShape docstring...")

        // .def("reserve",
        //     &DifferentShape::reserve,
        //     "reserve docstring...",
        //     py::return_value_policy::reference_internal)

        .def_property_readonly(
            "dtype",
            [](DifferentShape const& self)
            {
                py::object object = hdf5_type_id_to_numpy_dtype(
                    self.memory_datatype());
                assert(object.ptr() != nullptr);
                return object;
            },
            "dtype docstring..."
        )

        .def_property_readonly(
            "rank",
            &DifferentShape::rank,
            "rank docstring..."
        )

        .def(
            "__len__",
            &DifferentShape::nr_items,
            "__len__ docstring..."
        )

        .def(
            "__getitem__",
            [](DifferentShape const& collection,
                size_t const idx)
            {
                if(idx >= collection.nr_items()) {
                    throw py::index_error();
                }

                return collection[idx];
            })

        ;

}

}  // namespace constant
}  // namespace constant_size
}  // namespace lue
