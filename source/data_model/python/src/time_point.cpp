#include "lue/info/time/time_point.hpp"
#include "python_extension.hpp"
#include <pybind11/pybind11.h>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {
    namespace data_model {

        void init_time_point(py::module& module)
        {

            py::class_<TimePoint, LocationInTime>(
                module,
                "TimePoint",
                R"(
    TODO
)")

                ;
        }

    }  // namespace data_model
}  // namespace lue
