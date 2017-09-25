// #include "lue/c_api/hdf5/hdf5_attribute.h"
// #include "lue/cxx_api/hdf5/datatype.h"
#include "lue/hdf5/datatype_traits.hpp"
#include "lue/array.hpp"
#include "lue/py/conversion.hpp"
#include "lue/py/numpy.hpp"

#include <boost/format.hpp>

#include <pybind11/numpy.h>
#include <pybind11/stl.h>
#include <pybind11/pybind11.h>

// #include <numpy/arrayobject.h>
// #include <iostream>
// #include <iterator>


namespace py = pybind11;
using namespace pybind11::literals;

namespace lue {

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
    hdf5::Shape const& shape)
{
    std::vector<size_t> extents(shape.begin(), shape.end());

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


void update_hyperslab_by_index(
    size_t const dimension_idx,
    long index,
    hdf5::Shape const& array_shape,
    hdf5::Hyperslab& hyperslab,
    hdf5::Shape& slice_shape,
    size_t& nr_erased_dimensions)
{
    if(index < 0) {
        index = array_shape[dimension_idx] + index;
    }

    // Hyperslab in source array
    size_t const start = index;
    size_t const step = 1;
    size_t const slice_length = 1;

    hyperslab.start()[dimension_idx] = start;
    hyperslab.stride()[dimension_idx] = step;
    hyperslab.count()[dimension_idx] = slice_length;

    slice_shape[dimension_idx - nr_erased_dimensions] = slice_length;

    // The target array has a dimension less than the array being indexed
    assert(!slice_shape.empty());
    assert(slice_shape[dimension_idx - nr_erased_dimensions] == 1);
    slice_shape.erase(
        slice_shape.begin() + dimension_idx - nr_erased_dimensions);
    ++nr_erased_dimensions;
}


void update_hyperslab_by_slice(
    size_t const dimension_idx,
    py::slice const& slice,
    hdf5::Shape const& array_shape,
    hdf5::Hyperslab& hyperslab,
    hdf5::Shape& slice_shape,
    size_t const nr_erased_dimensions)
{
    // Hyperslab in source array
    size_t start, stop, step, slice_length;

    if(!slice.compute(
            array_shape[dimension_idx], &start, &stop, &step, &slice_length)) {
        throw py::error_already_set();
    }

    hyperslab.start()[dimension_idx] = start;
    hyperslab.stride()[dimension_idx] = step;
    hyperslab.count()[dimension_idx] = slice_length;

    slice_shape[dimension_idx - nr_erased_dimensions] = slice_length;
}


py::array create_array(
    hdf5::Datatype const& datatype,
    hdf5::Shape const& shape)
{
    py::array array;

    if(datatype == H5T_NATIVE_UINT32) {
        array = create_array<uint32_t>(shape);
    }
    else if(datatype == H5T_NATIVE_INT32) {
        array = create_array<int32_t>(shape);
    }
    else if(datatype == H5T_NATIVE_UINT64) {
        array = create_array<uint64_t>(shape);
    }
    else if(datatype == H5T_NATIVE_INT64) {
        array = create_array<int64_t>(shape);
    }
    else if(datatype == H5T_NATIVE_FLOAT) {
        array = create_array<float>(shape);
    }
    else if(datatype == H5T_NATIVE_DOUBLE) {
        array = create_array<double>(shape);
    }
    else {
        throw std::runtime_error(
            "Unsupported array value type");
    }

    return array;
}


py::array read_from_array(
    Array const& array,
    hdf5::Hyperslab const& hyperslab,
    hdf5::Shape const& slice_shape)
{
    // Read hyperslab from source array into target array
    auto result = create_array(array.datatype(), slice_shape);
    auto buffer = result.request().ptr;

    array.read(hyperslab, buffer);

    return result;
}


template<
    typename T>
void verify_write(
    Array const& /* array */,
    hdf5::Hyperslab const& /* hyperslab */)
{
    // TODO Verify hyperslab makes sense with array.



    // auto const array_info = values.request();

    // if(shape.size() != array_info.ndim) {
    //     throw std::runtime_error("rank of arrays differ");
    // }


    // if(hyperslab.count()[0] != array_info.shape[0]) {
    //     throw std::runtime_error(boost::str(boost::format(
    //         "Number of values to assign (%1%) must equal size "
    //         "of selected slice (%2%)")
    //             % array_info.shape[0]
    //             % hyperslab.count()[0]
    //         ));
    // }

    // // TODO Test hyperslab, not shape of array
    // // if(array_info.shape != shape) {
    // //     throw std::runtime_error(boost::str(boost::format(
    // //         "Shape of values to assign %1% must be equal to "
    // //         "shape of selected slice %2%")
    // //             % shape_as_string(array_info.shape)
    // //             % shape_as_string(shape)
    // //         ));
    // // }




    // if(hdf5::NativeDatatypeTraits<T>::type_id() != array.datatype()) {
    //     throw std::runtime_error(boost::str(boost::format(
    //         "Value type of value(s) to assign from (%1%) must equal "
    //         "the value type of the array to assign to (%2%)")
    //             % hdf5::NativeDatatypeTraits<T>::name()
    //             % hdf5::native_datatype_as_string(array.datatype())
    //         ));
    // }
}


template<
    typename T>
void write_to_array(
    Array& array,
    hdf5::Hyperslab const& hyperslab,
    T const value)
{
    verify_write<T>(array, hyperslab);

    // Write value into hyperslab target array
    hdf5::Datatype memory_datatype{hdf5::NativeDatatypeTraits<T>::type_id()};

    dynamic_cast<hdf5::Dataset&>(array).fill(
        memory_datatype, hyperslab, &value);
}


template<
    typename T>
void write_to_array(
    Array& array,
    hdf5::Hyperslab const& hyperslab,
    T const* values)
{
    verify_write<T>(array, hyperslab);

    // Write source array into hyperslab target array
    hdf5::Datatype memory_datatype{hdf5::NativeDatatypeTraits<T>::type_id()};
    dynamic_cast<hdf5::Dataset&>(array).write(
        memory_datatype, hyperslab, values);
}


template<
    typename T>
void write_to_array(
    Array& array,
    hdf5::Dataspace const& memory_dataspace,
    hdf5::Hyperslab const& hyperslab,
    T const* values)
{
    verify_write<T>(array, hyperslab);

    // Write source array into hyperslab target array
    hdf5::Datatype memory_datatype{hdf5::NativeDatatypeTraits<T>::type_id()};
    dynamic_cast<hdf5::Dataset&>(array).write(
        memory_datatype, memory_dataspace, hyperslab, values);
}


// // Required for comparing pybind11 shape and our shape. The value types of
// // these collections differ slightly (long long unsigned int versus long
// // unsigned int).
// template<
//     typename Collection1,
//     typename Collection2>
// bool operator==(
//     Collection1 const& collection1,
//     Collection2 const& collection2)
// {
//     return collection1.size() == collection2.size() &&
//         std::equal(collection1.begin(), collection1.end(),
//             collection2.begin());
// }
// 
// 
// template<
//     typename Collection1,
//     typename Collection2>
// bool operator!=(
//     Collection1 const& collection1,
//     Collection2 const& collection2)
// {
//     return !(collection1 == collection2);
// }
// 
// 
// template<
//     typename Collection>
// std::string shape_as_string(
//     Collection const& collection)
// {
//     std::stringstream stream;
//     stream << "(";
// 
//     // Prevent seperator at end of string.
//     // C++17: ostream_joiner
//     if(!collection.empty()) {
//         std::copy(collection.begin(), collection.end() - 1,
//             std::ostream_iterator<typename Collection::value_type>(stream,
//                 ", "));
//         stream << collection.back();
//     }
//     stream << ")";
// 
//     return stream.str();
// }




template<
    typename T>
void set_item(
    Array& array,
    std::int64_t const index,
    T const value)
{
    hdf5::Shape const shape{array.shape()};
    hdf5::Hyperslab hyperslab{shape};
    hdf5::Shape slice_shape{shape};
    size_t nr_erased_dimensions = 0;

    update_hyperslab_by_index(
        0, index, shape, hyperslab, slice_shape, nr_erased_dimensions);

    write_to_array(array, hyperslab, value);
}


template<
    typename T>
void set_item(
    Array& array,
    py::slice const& slice,
    T const value)
{
    hdf5::Shape const shape{array.shape()};
    hdf5::Hyperslab hyperslab{shape};
    hdf5::Shape slice_shape{shape};
    size_t nr_erased_dimensions = 0;

    update_hyperslab_by_slice(
        0, slice, shape, hyperslab, slice_shape, nr_erased_dimensions);

    write_to_array(array, hyperslab, value);
}


template<
    typename T>
void set_item(
    Array& array,
    std::int64_t const index,
    T const* values)
{
    hdf5::Shape const shape{array.shape()};
    hdf5::Hyperslab hyperslab{shape};
    hdf5::Shape slice_shape{shape};
    size_t nr_erased_dimensions = 0;

    update_hyperslab_by_index(
        0, index, shape, hyperslab, slice_shape, nr_erased_dimensions);

    write_to_array(array, hyperslab, values);
}


template<
    typename T>
void set_item(
    Array& array,
    std::int64_t const index,
    py::array_t<T, py::array::c_style>& values)
{
    set_item(array, index, static_cast<T const*>(values.request().ptr));
}


template<
    typename T>
void set_item(
    Array& array,
    hdf5::Dataspace const& memory_dataspace,
    py::slice const& slice,
    T const* values)
{
    hdf5::Shape const shape{array.shape()};
    hdf5::Hyperslab hyperslab{shape};
    hdf5::Shape slice_shape{shape};
    size_t nr_erased_dimensions = 0;

    update_hyperslab_by_slice(
        0, slice, shape, hyperslab, slice_shape, nr_erased_dimensions);

    write_to_array(array, memory_dataspace, hyperslab, values);
}


/// template<
///     typename T>
/// void set_item(
///     Array& array,
///     py::slice const& slice,
///     T const& value)
/// {
///     hdf5::Shape const sizes{1};
///     auto const memory_dataspace = hdf5::create_dataspace(sizes);
/// 
///     set_item(array, memory_dataspace, slice, &value);
/// }


template<
    typename T>
void set_item(
    Array& array,
    py::slice const& slice,
    py::array_t<T, py::array::c_style>& values)
{
    hdf5::Shape const sizes(values.shape(), values.shape() + values.ndim());
    auto const memory_dataspace = hdf5::create_dataspace(sizes);

    set_item(
        array, memory_dataspace, slice,
        static_cast<T const*>(values.request().ptr));
}


template<
    typename T>
void set_item(
    Array& /* array */,
    std::vector<py::object> const& /* indices */,
    py::array_t<T, py::array::c_style>& /* values */)
{
    assert(false);
}


void init_array_class(
    py::module& module)
{
    init_numpy();

    py::class_<
            Array,
            hdf5::Dataset>(
        module,
        "Array",
        "Array docstring...")

        .def_property_readonly(
            "dtype",
            [](Array const& self) {
                py::object object = hdf5_type_id_to_numpy_dtype(
                    self.memory_datatype());
                assert(object.ptr() != nullptr);
                return object;
            },
            "dtype docstring..."
        )

        .def("__getitem__", [](
                Array const& array,
                py::int_ const& index) -> py::array {

            hdf5::Shape const shape{array.shape()};
            hdf5::Hyperslab hyperslab(shape);
            hdf5::Shape slice_shape{shape};
            size_t nr_erased_dimensions = 0;

            update_hyperslab_by_index(
                0, index, shape, hyperslab, slice_shape, nr_erased_dimensions);

            return read_from_array(array, hyperslab, slice_shape);
        })

        .def("__getitem__", [](
                Array const& array,
                py::slice const& slice) -> py::array {

            hdf5::Shape const shape{array.shape()};
            hdf5::Hyperslab hyperslab(shape);
            hdf5::Shape slice_shape{shape};
            size_t nr_erased_dimensions = 0;

            update_hyperslab_by_slice(
                0, slice, shape, hyperslab, slice_shape, nr_erased_dimensions);

            return read_from_array(array, hyperslab, slice_shape);
        })

        .def("__getitem__", [](
                Array const& array,
                std::vector<py::object> const& indices) -> py::array {

            // With each index, a selection is made along the corresponding
            // dimension. The number of indices cannot be larger than
            // the rank of the array. It can be smaller, in which case
            // all values along the missing dimensions are selected.
            // Indices can be:
            // - a slice: selects a set of values
            // - an integer: selects a single value
            //
            // When an integer is used to select values along a dimension,
            // this dimension is removed from the result array.

            hdf5::Shape const shape{array.shape()};

            if(indices.size() > shape.size()) {
                throw pybind11::index_error("too many indices for array");
            }

            // Hyperslab in source array
            hdf5::Hyperslab hyperslab(shape);

            // Shape of target array
            hdf5::Shape slice_shape{shape};
            size_t nr_erased_dimensions = 0;


            for(size_t i = 0; i < indices.size(); ++i) {
                auto const& index = indices[i];

                // Strings are implicitly converted into a py::slice.
                // The characters are used for start:count:step. To filter
                // out strings, we test them first.
                try {
                    std::string string = py::cast<std::string>(index);
                    throw pybind11::index_error(
                        "only integers and slices (`:`) are valid indices");
                }
                catch(py::cast_error const&) {
                }


                try {
                    long index_ = py::cast<long>(index);

                    update_hyperslab_by_index(
                        i, index_, shape, hyperslab, slice_shape,
                        nr_erased_dimensions);

                    continue;
                }
                catch(py::cast_error const&) {
                }

                try {
                    py::slice slice = index;

                    update_hyperslab_by_slice(
                        i, slice, shape, hyperslab, slice_shape,
                        nr_erased_dimensions);

                    continue;
                }
                catch(py::cast_error const&) {
                }

                throw pybind11::index_error(
                    "only integers and slices (`:`) are valid indices");
            }

            return read_from_array(array, hyperslab, slice_shape);
        })

        // Catch-all for when the argument is not a supported index type
        .def("__getitem__", [](
                Array const& /* array */,
                py::object const& /* index */) -> py::array {

            throw pybind11::index_error(
                "only integers and slices (`:`) are valid indices");

        })


        .def("__setitem__", [](
                Array& array,
                std::int64_t const index,
                py::int_ const& value) {
            set_item(array, index, std::int64_t{value});
        })

        .def("__setitem__", [](
                Array& array,
                std::int64_t const index,
                py::float_ const& value) {
            set_item(array, index, double{value});
        })

        .def("__setitem__", [](
                Array& array,
                py::slice const& slice,
                py::int_ const& value) {
            set_item(array, slice, std::int64_t{value});
        })

        .def("__setitem__", [](
                Array& array,
                py::slice const& slice,
                py::float_ const& value) {
            set_item(array, slice, double{value});
        })


#define SETITEM_INTEGER(                                       \
            T)                                                 \
        .def("__setitem__", [](                                \
                Array& array,                                  \
                std::int64_t const index,                      \
                py::array_t<T, py::array::c_style>& values) {  \
            set_item<T>(array, index, values);                 \
        })


#define SETITEM_SLICE(                                         \
        T)                                                     \
        .def("__setitem__", [](                                \
                Array& array,                                  \
                py::slice const& slice,                        \
                py::array_t<T, py::array::c_style>& values) {  \
            set_item<T>(array, slice, values);                 \
        })


#define SETITEM_INDICES( \
        T) \
        .def("__setitem__", []( \
                Array& array, \
                std::vector<py::object> const& indices, \
                py::array_t<T, py::array::c_style>& values) { \
            set_item<T>(array, indices, values); \
})


        SETITEM_INTEGER(uint32_t)
        SETITEM_INTEGER(int32_t)
        SETITEM_INTEGER(uint64_t)
        SETITEM_INTEGER(int64_t)
        SETITEM_INTEGER(float)
        SETITEM_INTEGER(double)

#undef SET_ITEM_INTEGER

        SETITEM_SLICE(uint32_t)
        SETITEM_SLICE(int32_t)
        SETITEM_SLICE(uint64_t)
        SETITEM_SLICE(int64_t)
        SETITEM_SLICE(float)
        SETITEM_SLICE(double)

#undef SETITEM_SLICE

        SETITEM_INDICES(uint32_t)
        SETITEM_INDICES(int32_t)
        SETITEM_INDICES(uint64_t)
        SETITEM_INDICES(int64_t)
        SETITEM_INDICES(float)
        SETITEM_INDICES(double)

#undef SETITEM_INDICES

        ;

}

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


