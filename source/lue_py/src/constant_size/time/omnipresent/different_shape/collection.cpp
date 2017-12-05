#include "lue/item/constant_size/constant/different_shape.hpp"
#include "lue/py/conversion.hpp"
#include <pybind11/pybind11.h>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {
namespace constant_size {
namespace constant {
namespace different_shape {

void init_collection(
    py::module& module)
{

    py::class_<Collection, hdf5::Group>(
        module,
        "Collection",
        "Collection docstring...")

        // .def("reserve",
        //     &Collection::reserve,
        //     "reserve docstring...",
        //     py::return_value_policy::reference_internal)

        .def_property_readonly(
            "dtype",
            [](Collection const& self)
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
            &Collection::rank,
            "rank docstring..."
        )

        .def(
            "__len__",
            &Collection::nr_items,
            "__len__ docstring..."
        )

        .def(
            "__getitem__",
            [](Collection const& collection,
                size_t const idx)
            {
                if(idx >= collection.nr_items()) {
                    throw py::index_error();
                }

                return collection[idx];
            })

        ;

}

}  // namespace different_shape
}  // namespace constant
}  // namespace constant_size
}  // namespace lue
