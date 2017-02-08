// #include "lue/cxx_api/item.h"
// #include "lue/cxx_api/array.h"
#include "lue/cxx_api/define.h"
#include <pybind11/pybind11.h>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {
namespace python {

void init_item(
        py::module& module)
{

    py::enum_<ShapePerItem>(
            module,
            "shape_per_item",
            "shape_per_item docstring...")
        .value("same", ShapePerItem::same)
        .value("different", ShapePerItem::different)
    ;

    // py::class_<time::Item>(module,
    //     "Item", // py::base<hdf5::Group>(),
    //     "Item docstring...")
    // ;

    // py::class_<time::omnipresent::Item>(module,
    //     "O_Item", py::base<time::Item>(),
    //     "O_Item docstring...")
    // ;

}

}  // namespace python
}  // namespace lue
