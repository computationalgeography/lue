#include "lue/array/value_group.hpp"
#include "python_extension.hpp"
#include "lue/py/data_model/conversion.hpp"
#include <pybind11/pybind11.h>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {
    namespace data_model {

        void init_value_group(py::module& module)
        {

            py::class_<ValueGroup, hdf5::Group>(
                module,
                "ValueGroup",
                R"(
    ValueGroup docstring...
)")

                .def_property_readonly(
                    "dtype",
                    [](ValueGroup const& self)
                    {
                        py::object object = hdf5_type_id_to_numpy_dtype(self.memory_datatype());
                        assert(object.ptr() != nullptr);
                        return object;
                    },
                    "dtype docstring...")

                ;
        }

    }  // namespace data_model
}  // namespace lue
