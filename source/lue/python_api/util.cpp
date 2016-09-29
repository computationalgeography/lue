#include "lue/python_api/util.h"
#include "lue/cxx_api/hdf5/datatype.h"
#include <numpy/arrayobject.h>
#include "lue/python_api/numpy.h"


namespace py = pybind11;

namespace lue {
namespace python {

DEFINE_INIT_NUMPY()


pybind11::object hdf5_type_id_to_numpy_dtype(
    hid_t const type_id)
{
    init_numpy();

    std::string type_as_string;

    switch(H5Tget_class(type_id)) {
        case H5T_INTEGER: {
            if(datatypes_are_equal(type_id, H5T_NATIVE_UINT32)) {
                type_as_string = "uint32";
            }
            else if(datatypes_are_equal(type_id, H5T_NATIVE_INT32)) {
                type_as_string = "int32";
            }
            else if(datatypes_are_equal(type_id, H5T_NATIVE_UINT64)) {
                type_as_string = "uint64";
            }
            else if(datatypes_are_equal(type_id, H5T_NATIVE_INT64)) {
                type_as_string = "int64";
            }
            else {
                throw std::runtime_error(
                    "Unsupported integer self value type");
            }

            break;
        }
        case H5T_FLOAT: {
            if(datatypes_are_equal(type_id, H5T_NATIVE_FLOAT)) {
                type_as_string = "float32";
            }
            else if(datatypes_are_equal(type_id, H5T_NATIVE_DOUBLE)) {
                type_as_string = "float64";
            }
            else {
                throw std::runtime_error(
                    "Unsupported float array value type");
            }

            break;
        }
        default: {
            throw std::runtime_error(
                "Unsupported array value type");

            break;
        }
    }

    assert(!type_as_string.empty());

    py::object object(Py_BuildValue("s", type_as_string.c_str()), false);

    if(object.ptr() == nullptr) {
        throw py::error_already_set();
    }

    PyArray_Descr* dtype = nullptr;

    if(!PyArray_DescrConverter(object.ptr(), &dtype)) {
        throw py::error_already_set();
    }

    assert(dtype);

    return py::object(reinterpret_cast<PyObject*>(dtype), false);
}

}  // namespace python
}  // namespace lue
