#include "lue/py/conversion.hpp"
#include "lue/py/numpy.hpp"
#include <numpy/arrayobject.h>


namespace py = pybind11;

namespace lue {

DEFINE_INIT_NUMPY()


int numpy_type_id(
    py::handle const& numpy_type_id_object)
{
    init_numpy();

    int numpy_type_id = NPY_NOTYPE;

    {
        PyArray_Descr* dtype;
        if(!PyArray_DescrConverter(
                numpy_type_id_object.ptr(), &dtype)) {
            throw py::error_already_set();
        }
        numpy_type_id = dtype->type_num;
        Py_DECREF(dtype);
    }

    return numpy_type_id;
}


std::tuple<hid_t, hid_t> numpy_type_to_hdf5_types(
    int const type_id)
{
    hid_t file_type_id = -1;
    hid_t memory_type_id = -1;

    switch(type_id) {
        case NPY_UINT32: {
            file_type_id = H5T_STD_U32LE;
            memory_type_id = H5T_NATIVE_UINT32;
            break;
        }
        case NPY_INT32: {
            file_type_id = H5T_STD_I32LE;
            memory_type_id = H5T_NATIVE_INT32;
            break;
        }
        case NPY_UINT64: {
            file_type_id = H5T_STD_U64LE;
            memory_type_id = H5T_NATIVE_UINT64;
            break;
        }
        case NPY_INT64: {
            file_type_id = H5T_STD_I64LE;
            memory_type_id = H5T_NATIVE_INT64;
            break;
        }
        case NPY_FLOAT32: {
            file_type_id = H5T_IEEE_F32LE;
            memory_type_id = H5T_NATIVE_FLOAT;
            break;
        }
        case NPY_FLOAT64: {
            file_type_id = H5T_IEEE_F64LE;
            memory_type_id = H5T_NATIVE_DOUBLE;
            break;
        }
        default: {
            throw std::runtime_error("Unsupported numpy type");
            break;
        }
    }

    return std::make_tuple(file_type_id, memory_type_id);
}


hdf5::Datatype numpy_type_to_memory_datatype(
    int const type_id)
{
    hid_t file_type_id, memory_type_id;

    std::tie(file_type_id, memory_type_id) = numpy_type_to_hdf5_types(type_id);

    return hdf5::Datatype{memory_type_id};
}


hdf5::Datatype numpy_type_to_memory_datatype(
    py::handle const& numpy_type_id_object)
{
    auto const type_id = numpy_type_id(numpy_type_id_object);

    return numpy_type_to_memory_datatype(type_id);
}


pybind11::object hdf5_type_id_to_numpy_dtype(
    hdf5::Datatype const datatype)
{
    init_numpy();

    std::string type_as_string;

    if(datatype == hdf5::Datatype{H5T_NATIVE_UINT32}) {
        type_as_string = "uint32";
    }
    else if(datatype == hdf5::Datatype{H5T_NATIVE_INT32}) {
        type_as_string = "int32";
    }
    else if(datatype == hdf5::Datatype{H5T_NATIVE_UINT64}) {
        type_as_string = "uint64";
    }
    else if(datatype == hdf5::Datatype{H5T_NATIVE_INT64}) {
        type_as_string = "int64";
    }
    else if(datatype == hdf5::Datatype{H5T_NATIVE_FLOAT}) {
        type_as_string = "float32";
    }
    else if(datatype == hdf5::Datatype{H5T_NATIVE_DOUBLE}) {
        type_as_string = "float64";
    }
    else {
        throw std::runtime_error(
            "Unsupported array value type");
    }

    assert(!type_as_string.empty());

    PyObject* result = Py_BuildValue("s", type_as_string.c_str());

    if(!result) {
        throw py::error_already_set();
    }

    auto object = py::reinterpret_steal<py::object>(result);
    result = nullptr;

    PyArray_Descr* dtype = nullptr;

    if(!PyArray_DescrConverter(object.ptr(), &dtype)) {
        throw py::error_already_set();
    }

    assert(dtype);

    return py::reinterpret_steal<py::object>(
        reinterpret_cast<PyObject*>(dtype));
}


hdf5::Shape tuple_to_shape(
    py::tuple const& tuple)
{
    hdf5::Shape shape(tuple.size());

    for(size_t i = 0; i < tuple.size(); ++i) {
        shape[i] = py::int_(tuple[i]);
    }

    return shape;
}

}  // namespace lue
