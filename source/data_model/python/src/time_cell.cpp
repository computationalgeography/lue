#include "lue/info/time/time_cell.hpp"
#include "python_extension.hpp"
#include <pybind11/pybind11.h>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {
    namespace data_model {

        void init_time_cell(py::module& module)
        {

            py::class_<TimeCell, TimeBox>(
                module,
                "TimeCell",
                R"(
    TODO
)")

                .def_property_readonly("nr_counts", &TimeCell::nr_counts)

                .def_property_readonly(
                    "count",
                    py::overload_cast<>(&TimeCell::count),
                    py::return_value_policy::reference_internal)

                ;
        }

    }  // namespace data_model
}  // namespace lue
