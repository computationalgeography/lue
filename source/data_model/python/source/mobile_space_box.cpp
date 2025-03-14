#include "lue/info/space/mobile_space_box.hpp"
#include "python_extension.hpp"
#include <pybind11/pybind11.h>
#include <format>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {
    namespace data_model {
        namespace {

            static std::string formal_string_representation(MobileSpaceBox const& box)
            {
                return std::format("MobileSpaceBox(pathname='{}')", box.id().pathname());
            }


            static std::string informal_string_representation(MobileSpaceBox const& box)
            {
                return std::format(
                    "{}\n"
                    "    nr_boxes: {}",
                    formal_string_representation(box),
                    box.nr_boxes());
            }

        }  // Anonymous namespace

        void init_mobile_space_box(py::module& module)
        {

            py::class_<MobileSpaceBox, same_shape::constant_shape::Value>(
                module,
                "MobileSpaceBox",
                R"(
    TODO
)")

                .def("__repr__", [](MobileSpaceBox const& box) { return formal_string_representation(box); })

                .def("__str__", [](MobileSpaceBox const& box) { return informal_string_representation(box); })

                ;
        }

    }  // namespace data_model
}  // namespace lue
