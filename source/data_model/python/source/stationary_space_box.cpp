#include "lue/info/space/stationary_space_box.hpp"
#include "python_extension.hpp"
#include <pybind11/pybind11.h>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {
    namespace data_model {

        void init_stationary_space_box(py::module& module)
        {

            py::class_<StationarySpaceBox, same_shape::Value>(
                module,
                "StationarySpaceBox",
                R"(
    TODO
)")

                .def_property_readonly("nr_boxes", &StationarySpaceBox::nr_boxes)

                ;
        }

    }  // namespace data_model
}  // namespace lue
