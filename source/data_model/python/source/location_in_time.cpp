#include "lue/info/time/location_in_time.hpp"
#include "python_extension.hpp"
#include <pybind11/pybind11.h>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {
    namespace data_model {

        void init_location_in_time(py::module& module)
        {

            py::class_<LocationInTime, same_shape::constant_shape::Value>(
                module,
                "LocationInTime",
                R"(
    TODO
)")

                ;
        }

    }  // namespace data_model
}  // namespace lue
