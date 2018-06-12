#include "lue/item/different_shape/continuous_value.hpp"
#include "lue/py/conversion.hpp"
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>


namespace py = pybind11;


namespace lue {
namespace different_shape {

void init_continuous_value(
    py::module& module)
{

    py::class_<ContinuousValue, hdf5::Group>(
        module,
        "ContinuousValue",
        "ContinuousValue docstring...")

        .def(
            "reserve",
            [](
                ContinuousValue& self,
                py::array_t<hsize_t, py::array::c_style>& shapes) -> ContinuousValue&
            {
                static_assert(sizeof(hsize_t) == sizeof(uint64_t), "");

                // shapes must be an nD array where:
                // - the number of dimensions must equal rank + 1
                // - the first dimension corresponds with the nr_items
                // - subsequent dimensions correspond with the extents of
                //   each item's value

                auto const array_info = shapes.request();

                // if(static_cast<rank_t>(array_info.ndim) != 2) {
                if(array_info.ndim != 2) {
                    throw std::runtime_error(
                        "rank of shapes array (" +
                        std::to_string(array_info.ndim) + ") must equal 2");
                }

                if(static_cast<int>(array_info.shape[1]) != self.rank()) {
                    throw std::runtime_error(
                        "extent of second dimension of shapes array (" +
                        std::to_string(array_info.shape[1]) +
                        ") must equal rank of values (" +
                        std::to_string(self.rank()) + ")");
                }

                hsize_t const nr_items = array_info.shape[0];

                self.reserve(
                    nr_items, static_cast<hsize_t const*>(array_info.ptr));

                return self;
            },
            "reserve docstring...",
            py::return_value_policy::reference_internal)

        .def_property_readonly(
            "dtype",
            [](ContinuousValue const& self)
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
            &ContinuousValue::rank,
            "rank docstring..."
        )

        .def(
            "__len__",
            &ContinuousValue::nr_items,
            "__len__ docstring..."
        )

        .def(
            "__getitem__",
            [](
                ContinuousValue const& self,
                size_t const idx)
            {
                if(idx >= self.nr_items()) {
                    throw py::index_error();
                }

                return self[idx];
            })

        ;

}

}  // namespace different_shape
}  // namespace lue
