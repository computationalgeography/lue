#include "lue/c_api/hdf5/hdf5_attribute.h"
#include "lue/cxx_api/hdf5/datatype.h"
#include "lue/cxx_api/hdf5/datatype_traits.h"
#include "lue/cxx_api/array.h"
#include "lue/python_api/numpy.h"

#include <boost/format.hpp>

#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
// #include <pybind11/stl.h>

#include <numpy/arrayobject.h>
#include <iostream>
#include <iterator>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {
namespace python {

DEFINE_INIT_NUMPY()


/*
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
*/


template<
    typename T>
py::array create_array(
    Shape const& shape)
{
    std::vector<size_t> extents(shape.size());
    std::copy(shape.begin(), shape.end(), extents.begin());

    // Fill collection with strides. E.g., in case of shape [500, 4, 5],
    // the strides collection must be [20, 5, 1].
    std::vector<size_t> strides(shape.size());
    size_t stride = 1;
    std::transform(shape.rbegin(), shape.rend(), strides.begin(),
        [&stride](T const& v) {
            size_t tmp_stride = stride;
            stride *= v;
            return tmp_stride;
        }
    );
    std::reverse(strides.begin(), strides.end());
    std::transform(strides.begin(), strides.end(), strides.begin(), [](
                T const& v) {
            return v * sizeof(T);
        }
    );

    return py::array(py::buffer_info(
        nullptr,                          // Ask numpy to allocate
        sizeof(T),                        // Size of one item
        py::format_descriptor<T>::value,  // Buffer format
        shape.size(),                     // Rank
        extents,
        strides
    ));
}


// Required for comparing pybind11 shape and our shape. The value types of
// these collections differ slightly (long long unsigned int versus long
// unsigned int).
template<
    typename Collection1,
    typename Collection2>
bool operator==(
    Collection1 const& collection1,
    Collection2 const& collection2)
{
    return collection1.size() == collection2.size() &&
        std::equal(collection1.begin(), collection1.end(),
            collection2.begin());
}


template<
    typename Collection1,
    typename Collection2>
bool operator!=(
    Collection1 const& collection1,
    Collection2 const& collection2)
{
    return !(collection1 == collection2);
}


template<
    typename Collection>
std::string shape_as_string(
    Collection const& collection)
{
    std::stringstream stream;
    stream << "(";

    // Prevent seperator at end of string.
    // C++17: ostream_joiner
    if(!collection.empty()) {
        std::copy(collection.begin(), collection.end() - 1,
            std::ostream_iterator<typename Collection::value_type>(stream,
                ", "));
        stream << collection.back();
    }
    stream << ")";

    return stream.str();
}


template<
    typename T>
void set_item(
    Array& array,
    py::slice const& slice,
    py::array_t<T, py::array::c_style>& values)
{
    Shape const shape{array.shape()};

    size_t start, stop, step, slice_length;

    if(!slice.compute(shape[0], &start, &stop, &step, &slice_length)) {
        throw py::error_already_set();
    }

    auto const array_info = values.request();

    if(shape.size() != array_info.ndim) {
        throw std::runtime_error("rank of arrays differ");
    }


    if(!datatypes_are_equal(NativeDatatypeTraits<T>::type_id(),
            array.type_id())) {
        throw std::runtime_error(boost::str(boost::format(
            "Value type of array to assign from (%1%) must equal "
            "the value type of the array to assign to (%2%)")
                % NativeDatatypeTraits<T>::name()
                % native_datatype_as_string(array.type_id())
                // % hdf5_type_name(array.type_id())
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

    // TODO Test hyperslab, not shape of array
    // if(array_info.shape != shape) {
    //     throw std::runtime_error(boost::str(boost::format(
    //         "Shape of values to assign %1% must be equal to "
    //         "shape of selected slice %2%")
    //             % shape_as_string(array_info.shape)
    //             % shape_as_string(shape)
    //         ));
    // }

    std::vector<hsize_t> hyperslab_start(shape.size(), 0);
    hyperslab_start[0] = start;

    std::vector<hsize_t> hyperslab_count(shape);
    hyperslab_count[0] = slice_length;

    std::vector<hsize_t> hyperslab_stride(shape.size(), 1);

    array.write(hyperslab_start, hyperslab_count, hyperslab_stride,
        static_cast<T*>(array_info.ptr));
}


void init_array(
        py::module& module)
{
    init_numpy();

    py::class_<Shape>(module, "Shape",
        "Shape docstring...")

        .def("__repr__",
            [](Shape const& shape) {
                return shape_as_string(shape);
            }
        )

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


#define SETITEM( \
        T) \
.def("__setitem__", []( \
        Array& array, \
        py::slice const& slice, \
        py::array_t<T, py::array::c_style>& values) { \
    set_item<T>(array, slice, values); \
})

    py::class_<Array>(module, "Array",
        "Array docstring...")

        .def_property_readonly("shape", &Array::shape,
            "shape docstring...")
            // py::return_value_policy::reference_internal)

        .def_property_readonly("dtype", [](
                    Array const& self) {
                py::object object = hdf5_type_id_to_numpy_dtype(
                    self.type_id());
                assert(object.ptr() != nullptr);
                return object;
            },
            "dtype docstring..."
        )

        .def("__getitem__", [](
                Array const& array,
                py::slice const& slice) -> py::array {

            Shape const shape{array.shape()};

            size_t start, stop, step, slice_length;

            if(!slice.compute(shape[0], &start, &stop, &step,
                    &slice_length)) {
                throw py::error_already_set();
            }

            std::vector<hsize_t> hyperslab_start(shape.size(), 0);
            hyperslab_start[0] = start;

            std::vector<hsize_t> hyperslab_count(shape);
            hyperslab_count[0] = slice_length;

            std::vector<hsize_t> hyperslab_stride(shape.size(), 1);

            Shape slice_shape{shape};
            slice_shape[0] = slice_length;

            // Create numpy array.
            py::array result;

            switch(H5Tget_class(array.type_id())) {
                case H5T_INTEGER: {
                    if(datatypes_are_equal(array.type_id(),
                            H5T_NATIVE_UINT32)) {
                        result = create_array<uint32_t>(slice_shape);
                    }
                    else if(datatypes_are_equal(array.type_id(),
                            H5T_NATIVE_INT32)) {
                        result = create_array<int32_t>(slice_shape);
                    }
                    else if(datatypes_are_equal(array.type_id(),
                            H5T_NATIVE_UINT64)) {
                        result = create_array<uint64_t>(slice_shape);
                    }
                    else if(datatypes_are_equal(array.type_id(),
                            H5T_NATIVE_INT64)) {
                        result = create_array<int64_t>(slice_shape);
                    }
                    else {
                        throw std::runtime_error(
                            "Unsupported integer array value type");
                    }
                    break;
                }
                case H5T_FLOAT: {
                    if(datatypes_are_equal(array.type_id(),
                            H5T_NATIVE_FLOAT)) {
                        result = create_array<float>(slice_shape);
                    }
                    else if(datatypes_are_equal(array.type_id(),
                            H5T_NATIVE_DOUBLE)) {
                        result = create_array<double>(slice_shape);
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

            array.read(hyperslab_start, hyperslab_count, hyperslab_stride,
                buffer);

            return result;
        })

        SETITEM(uint32_t)
        SETITEM(int32_t)
        SETITEM(uint64_t)
        SETITEM(int64_t)
        SETITEM(float)
        SETITEM(double)

    ;

#undef SETITEM

}

}  // namespace python
}  // namespace lue



// std::string hdf5_type_name(
//         hid_t type_id)
// {
//     std::string result;
// 
//     switch(H5Tget_class(type_id)) {
//         case H5T_INTEGER: {
//             if(H5Tequal(type_id, H5T_NATIVE_UINT32)) {
//                 result = "uint32";
//             }
//             else if(H5Tequal(type_id, H5T_NATIVE_INT32)) {
//                 result = "int32";
//             }
//             else if(H5Tequal(type_id, H5T_NATIVE_UINT64)) {
//                 result = "uint64";
//             }
//             else if(H5Tequal(type_id, H5T_NATIVE_INT64)) {
//                 result = "int64";
//             }
//             else {
//                 throw std::runtime_error(
//                     "Unsupported integer array value type");
//             }
//             break;
//         }
//         case H5T_FLOAT: {
//             if(H5Tequal(type_id, H5T_NATIVE_FLOAT)) {
//                 result = "float32";
//             }
//             else if(H5Tequal(type_id, H5T_NATIVE_DOUBLE)) {
//                 result = "float64";
//             }
//             else {
//                 throw std::runtime_error(
//                     "Unsupported float array value type");
//             }
//             break;
//         }
//         default: {
//             throw std::runtime_error(
//                 "Unsupported array value type");
//             break;
//         }
//     }
// 
//     return result;
// }


