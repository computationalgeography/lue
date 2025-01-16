#include "lue/info/time/time_box.hpp"
#include "python_extension.hpp"
#include <pybind11/pybind11.h>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {
    namespace data_model {

        void init_time_box(py::module& module)
        {

            py::class_<TimeBox, LocationInTime>(
                module,
                "TimeBox",
                R"(
    TODO
)")

                .def_property_readonly("nr_boxes", &TimeBox::nr_boxes)

                ;
        }

    }  // namespace data_model
}  // namespace lue
