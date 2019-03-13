#include "submodule.hpp"
#include "lue/hdf5/shape.hpp"
#include <pybind11/stl_bind.h>


namespace py = pybind11;

PYBIND11_MAKE_OPAQUE(std::vector<hsize_t>)


namespace lue {
namespace hdf5 {

void init_shape(
    py::module& module)
{

    py::bind_vector<std::vector<hsize_t>>(
        module,
        "VectorHSizeT")

        ;


    py::class_<Shape, std::vector<hsize_t>>(
        module,
        "Shape",
        R"(
    Shape docstring...
)")

        ;

}

}  // namespace hdf5
}  // namespace lue
