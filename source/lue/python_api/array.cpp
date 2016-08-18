#include "lue/c_api/hdf5/hdf5_attribute.h"
#include "lue/cxx_api/array.h"
#include "lue/python_api/numpy.h"

#include <boost/format.hpp>

#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
// #include <pybind11/stl.h>

#include <numpy/arrayobject.h>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {
namespace python {

DEFINE_INIT_NUMPY()


template<
    typename T>
struct TypeTraits
{
    // static hid_t hdf5_type_id;
};


#define TYPE_TRAITS(  \
        type,  \
        hdf5_type_id_,  \
        name_)  \
template<>  \
struct TypeTraits<type>  \
{  \
    static hid_t hdf5_type_id() { return hdf5_type_id_; }  \
    static std::string name() { return name_; } \
};

TYPE_TRAITS(int32_t, H5T_NATIVE_INT32, "int32")
TYPE_TRAITS(int64_t, H5T_NATIVE_INT64, "int64")
TYPE_TRAITS(uint32_t, H5T_NATIVE_UINT32, "uint32")
TYPE_TRAITS(uint64_t, H5T_NATIVE_UINT64, "uint64")
TYPE_TRAITS(float, H5T_NATIVE_FLOAT, "float32")
TYPE_TRAITS(double, H5T_NATIVE_DOUBLE, "float64")

#undef TYPE_TRAITS


template<
    typename T>
py::array create_array(
    Shape const& shape,
    size_t slice_length)
{
    return py::array(py::buffer_info(
        nullptr,                          // Ask numpy to allocate
        sizeof(T),                        // Size of one item
        py::format_descriptor<T>::value,  // Buffer format
        shape.size(),                     // Rank
        { slice_length },                 // Extents
        { sizeof(T) }                     // Strides
    ));
}


std::string hdf5_type_name(
        hid_t type_id)
{
    std::string result;

    switch(H5Tget_class(type_id)) {
        case H5T_INTEGER: {
            if(H5Tequal(type_id, H5T_NATIVE_UINT32)) {
                result = "uint32";
            }
            else if(H5Tequal(type_id, H5T_NATIVE_INT32)) {
                result = "int32";
            }
            else if(H5Tequal(type_id, H5T_NATIVE_UINT64)) {
                result = "uint64";
            }
            else if(H5Tequal(type_id, H5T_NATIVE_INT64)) {
                result = "int64";
            }
            else {
                throw std::runtime_error(
                    "Unsupported integer array value type");
            }
            break;
        }
        case H5T_FLOAT: {
            if(H5Tequal(type_id, H5T_NATIVE_FLOAT)) {
                result = "float32";
            }
            else if(H5Tequal(type_id, H5T_NATIVE_DOUBLE)) {
                result = "float64";
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

    return result;
}


template<
    typename T>
void set_item(
    Array& array,
    py::slice const& slice,
    py::array_t<T, py::array::c_style>& values) {

    // TODO verify array can handle T's.

    // This overload is intended for 1D arrays.

    // Verify that
    // - the destination array is 1D
    // - the slice selects within the array bounds
    // - the source array is 1D
    // - the length of the source array corresponds with the length
    //   of the selection

    Shape const shape{array.shape()};

    if(shape.size() != 1) {
        throw std::runtime_error("1D destination array expected");
    }

    size_t start, stop, step, slice_length;

    if(!slice.compute(shape[0], &start, &stop, &step, &slice_length)) {
        throw py::error_already_set();
    }

    auto const array_info = values.request();

    if(array_info.ndim != 1) {
        throw std::runtime_error("1D source array expected");
    }

    if(!H5Tequal(TypeTraits<T>::hdf5_type_id(), array.type_id())) {
        throw std::runtime_error(boost::str(boost::format(
            "Value type of array to assign from (%1%) must equal "
            "the value type of the array to assign to (%2%)")
                % TypeTraits<T>::name()
                % hdf5_type_name(array.type_id())
            ));
    }

    if(slice_length != array_info.shape[0]) {
        throw std::runtime_error(boost::str(boost::format(
            "Number of values to assign (%1%) must equal size "
            "of selected slice (%2%)")
                % array_info.shape[0]
                % slice_length
            ));
    }

    array.write(start, slice_length, step, static_cast<T*>(array_info.ptr));
}


void init_array(
        py::module& module)
{
    init_numpy();

    py::class_<Shape>(module, "Shape",
        "Shape docstring...")
        .def("__len__", &Shape::size,
            "__len__ docstring...")
        .def("__getitem__", [](
                Shape const& shape,
                size_t const idx) {
            if(idx >= shape.size()) {
                throw py::index_error();
            }
            return shape[idx];
        })
    ;

    py::class_<Array>(module, "Array",
        "Array docstring...")

        .def_property_readonly("shape", &Array::shape,
            "shape docstring...")
            // py::return_value_policy::reference_internal)
        .def_property_readonly("dtype", [](
                    Array const& self) {

                std::string type_as_string;

                switch(H5Tget_class(self.type_id())) {
                    case H5T_INTEGER: {
                        if(H5Tequal(self.type_id(), H5T_NATIVE_UINT32)) {
                            type_as_string = "uint32";
                        }
                        else if(H5Tequal(self.type_id(), H5T_NATIVE_INT32)) {
                            type_as_string = "int32";
                        }
                        else if(H5Tequal(self.type_id(), H5T_NATIVE_UINT64)) {
                            type_as_string = "uint64";
                        }
                        else if(H5Tequal(self.type_id(), H5T_NATIVE_INT64)) {
                            type_as_string = "int64";
                        }
                        else {
                            throw std::runtime_error(
                                "Unsupported integer self value type");
                        }
                        break;
                    }
                    case H5T_FLOAT: {
                        if(H5Tequal(self.type_id(), H5T_NATIVE_FLOAT)) {
                            type_as_string = "float32";
                        }
                        else if(H5Tequal(self.type_id(), H5T_NATIVE_DOUBLE)) {
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

                py::object object(Py_BuildValue("s", type_as_string.c_str()),
                    false);

                if(object.ptr() == nullptr) {
                    throw py::error_already_set();
                }

                PyArray_Descr* dtype;
                if(!PyArray_DescrConverter(object.ptr(), &dtype)) {
                    throw py::error_already_set();
                }

                return py::object(reinterpret_cast<PyObject*>(dtype), false);
            },
            "dtype docstring..."
        )
        .def("__getitem__", [](
                Array const& array,
                py::slice const& slice) -> py::array {

            // This overload is intended for 1D arrays.

            Shape const shape{array.shape()};

            if(shape.size() != 1) {
                throw std::runtime_error("1D source array expected");
            }

            size_t start, stop, step, slice_length;

            if(!slice.compute(shape[0], &start, &stop, &step, &slice_length)) {
                throw py::error_already_set();
            }


            // Create numpy array.
            py::array result;

            switch(H5Tget_class(array.type_id())) {
                case H5T_INTEGER: {
                    if(H5Tequal(array.type_id(), H5T_NATIVE_UINT32)) {
                        result = create_array<uint32_t>(shape, slice_length);
                    }
                    else if(H5Tequal(array.type_id(), H5T_NATIVE_INT32)) {
                        result = create_array<int32_t>(shape, slice_length);
                    }
                    else if(H5Tequal(array.type_id(), H5T_NATIVE_UINT64)) {
                        result = create_array<uint64_t>(shape, slice_length);
                    }
                    else if(H5Tequal(array.type_id(), H5T_NATIVE_INT64)) {
                        result = create_array<int64_t>(shape, slice_length);
                    }
                    else {
                        throw std::runtime_error(
                            "Unsupported integer array value type");
                    }
                    break;
                }
                case H5T_FLOAT: {
                    if(H5Tequal(array.type_id(), H5T_NATIVE_FLOAT)) {
                        result = create_array<float>(shape, slice_length);
                    }
                    else if(H5Tequal(array.type_id(), H5T_NATIVE_DOUBLE)) {
                        result = create_array<double>(shape, slice_length);
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

            auto buffer = result.request().ptr;

            array.read(start, slice_length, step, buffer);

            return result;
        })
        .def("__setitem__", [](
                Array& array,
                py::slice const& slice,
                py::array_t<uint32_t, py::array::c_style>& values) {
            set_item<uint32_t>(array, slice, values);
        })
        .def("__setitem__", [](
                Array& array,
                py::slice const& slice,
                py::array_t<int32_t, py::array::c_style>& values) {
            set_item<int32_t>(array, slice, values);
        })
        .def("__setitem__", [](
                Array& array,
                py::slice const& slice,
                py::array_t<uint64_t, py::array::c_style>& values) {
            set_item<uint64_t>(array, slice, values);
        })
        .def("__setitem__", [](
                Array& array,
                py::slice const& slice,
                py::array_t<int64_t, py::array::c_style>& values) {
            set_item<int64_t>(array, slice, values);
        })
        .def("__setitem__", [](
                Array& array,
                py::slice const& slice,
                py::array_t<float, py::array::c_style>& values) {
            set_item<float>(array, slice, values);
        })
        .def("__setitem__", [](
                Array& array,
                py::slice const& slice,
                py::array_t<double, py::array::c_style>& values) {
            set_item<double>(array, slice, values);
        })
    ;

}

}  // namespace python
}  // namespace lue
