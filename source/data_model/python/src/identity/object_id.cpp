#include "lue/info/identity/object_id.hpp"
#include "../python_extension.hpp"
#include <pybind11/pybind11.h>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {
    namespace data_model {

        void init_object_id(py::module& module)
        {

            py::class_<ObjectID, same_shape::Value>(
                module,
                "ObjectID",
                R"(
    A class for storing the IDs of objects whose state does not change
    through time

    Use this class when object state is stored that does not change
    through time. The order of the IDs stored must match the order of the
    state stored. For example, when storing tree stem locations, the order
    of space points stored in the space domain must match the order of
    object IDs in the :class:`ObjectID` instance.

    You never have to create an :class:`ObjectID` instance
    yourself. :class:`Phenomenon` instances provide one.
)")

                ;
        }

    }  // namespace data_model
}  // namespace lue
