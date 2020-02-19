#include "../python_extension.hpp"
#include "lue/hdf5/datatype_traits.hpp"
#include "lue/hdf5/vlen_memory.hpp"
#include "lue/core/array.hpp"
#include "lue/core/define.hpp"
#include "lue/core/time/tick_period.hpp"
#include "lue/py/conversion.hpp"
#include "lue/py/numpy.hpp"
#include <fmt/format.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>
#include <pybind11/pybind11.h>
#include <boost/algorithm/string/trim.hpp>


namespace py = pybind11;
using namespace pybind11::literals;

namespace lue {
namespace data_model {

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


static std::vector<size_t> create_strides(
    hdf5::Shape const& shape,
    std::size_t size_of_element)
{
    assert(!shape.empty());

    std::vector<size_t> extents(shape.begin(), shape.end());

    // Fill collection with strides. E.g., in case of shape [500, 4, 5],
    // the strides collection must be [20, 5, 1].
    std::vector<std::size_t> strides(shape.size());
    std::size_t stride = 1;
    std::transform(shape.rbegin(), shape.rend(), strides.begin(),
        [&stride](std::size_t const& v) {
            size_t tmp_stride = stride;
            stride *= v;
            return tmp_stride;
        }
    );
    std::reverse(strides.begin(), strides.end());
    std::transform(strides.begin(), strides.end(), strides.begin(),
        [size_of_element](std::size_t const& v) {
            return v * size_of_element;
        }
    );

    return strides;
}


template<
    typename T>
py::array create_array(
    hdf5::Shape const& shape)
{
    // assert(!shape.empty());

    // std::vector<size_t> extents(shape.begin(), shape.end());

    // // Fill collection with strides. E.g., in case of shape [500, 4, 5],
    // // the strides collection must be [20, 5, 1].
    // std::vector<size_t> strides(shape.size());
    // size_t stride = 1;
    // std::transform(shape.rbegin(), shape.rend(), strides.begin(),
    //     [&stride](T const& v) {
    //         size_t tmp_stride = stride;
    //         stride *= v;
    //         return tmp_stride;
    //     }
    // );
    // std::reverse(strides.begin(), strides.end());
    // std::transform(strides.begin(), strides.end(), strides.begin(), [](
    //             T const& v) {
    //         return v * sizeof(T);
    //     }
    // );

    // return py::array_t<T>(shape, strides);

    return py::array_t<T>(shape, create_strides(shape, sizeof(T)));
}


static void update_hyperslab_by_index(
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

    if(static_cast<hdf5::Shape::value_type>(index) >= array_shape[0]) {
        throw pybind11::index_error(fmt::format(
                "index {} outside of range of valid indices [0, {})",
                index, array_shape[0]
            ));
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


static void update_hyperslab_by_slice(
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


static py::array create_array(
    Array const& array,
    hdf5::Hyperslab const& hyperslab,
    hdf5::Shape const& slice_shape)
{
    assert(!slice_shape.empty());

    py::array result;
    auto const& datatype = array.datatype();

    if(datatype.is_string()) {
        // HDF5 array contains variable length UTF8 strings. The
        // destination array must contain fixed length Unicode strings
        // (measured in code points). Shorter strings are padded with
        // nulls.

        assert(slice_shape.size() == 1);
        auto const nr_strings = slice_shape[0];

        // Read the strings in a data structure of our own
        auto const memory_datatype{lue::hdf5::create_string_datatype()};

        char* values_read[nr_strings];
        lue::hdf5::VLenMemory vlen{
            memory_datatype, array.dataspace(), values_read};

        array.read(hyperslab, values_read);

        // We now have variable length UTF8 encoded Unicode strings in
        // values_read. These must end up as fixed length Unicode strings
        // in the Numpy array.

        std::size_t max_nr_bytes_utf8 = 0;

        for(size_t i = 0; i < nr_strings; ++i) {
            max_nr_bytes_utf8 =
                std::max(max_nr_bytes_utf8, strlen(values_read[i]));
        }

        // Number of bytes used to represent a code point
        std::size_t const nr_bytes_per_code_point = 4;

        // Number of code points per string
        std::size_t const nr_code_points = max_nr_bytes_utf8;

        // Number of bytes per string
        std::size_t const nr_bytes_unicode =
            nr_code_points * nr_bytes_per_code_point;

        // Number of ordinals per string, to store the encoded Unicode
        // string on the Python side
        assert(nr_bytes_unicode % sizeof(Py_UNICODE) == 0);
        std::size_t const nr_ordinals = nr_bytes_unicode / sizeof(Py_UNICODE);

        // Buffer for all Unicode strings, back to back
        auto py_buffer = std::make_unique<Py_UNICODE[]>(
            nr_strings * nr_ordinals);
        std::fill(
            py_buffer.get(), py_buffer.get() + nr_strings * nr_ordinals, '\0');

        py::str py_string;
        Py_ssize_t data_size;
        char const* data;
        Py_UNICODE* it = py_buffer.get();

        for(std::size_t i = 0; i < nr_strings; ++i) {

            // Decode UTF8 string as read from LUE array
            py_string = py::reinterpret_steal<py::str>(
                ::PyUnicode_FromString(values_read[i]));

            if(!py_string) {
                throw py::error_already_set();
            }

            // Copy Unicode string into the slot in the Numpy array
            data_size = PyUnicode_GET_DATA_SIZE(py_string.ptr());  // Bytes
            data = PyUnicode_AS_DATA(py_string.ptr());
            std::memcpy(it, data, data_size);
            it += nr_ordinals;
        }

        py::capsule free_when_done(py_buffer.get(), [](void* buffer)
            {
                delete[] static_cast<Py_UNICODE*>(buffer);
            });

        py::dtype dtype{"U" + std::to_string(nr_code_points)};
        hdf5::Shape shape{nr_strings};
        result = py::array(dtype, shape, py_buffer.release(), free_when_done);
    }
    else {
        if(datatype == hdf5::Datatype{H5T_NATIVE_UINT8}) {
            result = create_array<uint8_t>(slice_shape);
        }
        else if(datatype == hdf5::Datatype{H5T_NATIVE_INT8}) {
            result = create_array<int8_t>(slice_shape);
        }
        else if(datatype == hdf5::Datatype{H5T_NATIVE_UINT32}) {
            result = create_array<uint32_t>(slice_shape);
        }
        else if(datatype == hdf5::Datatype{H5T_NATIVE_INT32}) {
            result = create_array<int32_t>(slice_shape);
        }
        else if(datatype == hdf5::Datatype{H5T_NATIVE_UINT64}) {
            result = create_array<uint64_t>(slice_shape);
        }
        else if(datatype == hdf5::Datatype{H5T_NATIVE_INT64}) {
            result = create_array<int64_t>(slice_shape);
        }
        else if(datatype == hdf5::Datatype{H5T_NATIVE_FLOAT}) {
            result = create_array<float>(slice_shape);
        }
        else if(datatype == hdf5::Datatype{H5T_NATIVE_DOUBLE}) {
            result = create_array<double>(slice_shape);
        }
        else {
            throw std::runtime_error(
                "Unsupported array value type");
        }

        array.read(hyperslab, result.request().ptr);
    }

    return result;
}


template<
    typename T>
py::object create_scalar(
    Array const& array,
    hdf5::Hyperslab const& hyperslab)
{
    T value;

    array.read(hyperslab, &value);

    return py::reinterpret_steal<py::object>(PyArray_Scalar(
        &value,
        (PyArray_Descr*)py::dtype::of<T>().ptr(),
        py::int_(sizeof(T)).ptr()));
}


static py::object create_scalar(
    Array const& array,
    hdf5::Hyperslab const& hyperslab)
{
    assert(hyperslab.nr_elements() == 1);

    py::object result;
    auto const& datatype = array.datatype();

    if(datatype == hdf5::Datatype{H5T_NATIVE_UINT8}) {
        result = create_scalar<uint8_t>(array, hyperslab);
    }
    else if(datatype == hdf5::Datatype{H5T_NATIVE_INT8}) {
        result = create_scalar<int8_t>(array, hyperslab);
    }
    else if(datatype == hdf5::Datatype{H5T_NATIVE_UINT32}) {
        result = create_scalar<uint32_t>(array, hyperslab);
    }
    else if(datatype == hdf5::Datatype{H5T_NATIVE_INT32}) {
        result = create_scalar<int32_t>(array, hyperslab);
    }
    else if(datatype == hdf5::Datatype{H5T_NATIVE_UINT64}) {
        result = create_scalar<uint64_t>(array, hyperslab);
    }
    else if(datatype == hdf5::Datatype{H5T_NATIVE_INT64}) {
        result = create_scalar<int64_t>(array, hyperslab);
    }
    else if(datatype == hdf5::Datatype{H5T_NATIVE_FLOAT}) {
        result = create_scalar<float>(array, hyperslab);
    }
    else if(datatype == hdf5::Datatype{H5T_NATIVE_DOUBLE}) {
        result = create_scalar<double>(array, hyperslab);
    }
    else {
        throw std::runtime_error(
            "Unsupported array value type");
    }

    return result;
}


static py::object read_hyperslab_from_array(
    Array const& array,
    hdf5::Hyperslab const& hyperslab,
    hdf5::Shape const& slice_shape)
{
    return slice_shape.empty()
        ? create_scalar(array, hyperslab)
        : create_array(array, hyperslab, slice_shape);
        ;
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


void init_array(
    py::module& module)
{
    init_numpy();

    {
        py::module submodule = module.def_submodule(
            "dtype",
            R"(
    TODO
)");

        submodule.attr("ID") = py::dtype::of<ID>();
        submodule.attr("Index") = py::dtype::of<Index>();
        submodule.attr("Count") = py::dtype::of<Count>();
        submodule.attr("TickPeriodCount") =
            py::dtype::of<time::TickPeriodCount>();
    }

    py::class_<
            Array,
            hdf5::Dataset>(
        module,
        "Array",
        "Array docstring...")

        .def_property_readonly(
            "dtype",
            [](Array const& self)
            {
                py::object object = hdf5_type_id_to_numpy_dtype(
                    self.memory_datatype());
                assert(object.ptr() != nullptr);
                return object;
            },
            "dtype docstring..."
        )

        .def("__iter__", [](Array const& /* array */) {
            throw std::invalid_argument(
                "on the fly iteration not supported, use the numpy array");
        })

        .def("__getitem__", [](
                Array const& array,
                py::int_ const& index) -> py::object {

            hdf5::Shape const shape{array.shape()};
            hdf5::Hyperslab hyperslab(shape);
            hdf5::Shape slice_shape{shape};
            size_t nr_erased_dimensions = 0;

            update_hyperslab_by_index(
                0, index, shape, hyperslab, slice_shape, nr_erased_dimensions);

            return read_hyperslab_from_array(array, hyperslab, slice_shape);
        })

        .def("__getitem__", [](
                Array const& array,
                py::slice const& slice) -> py::object {

            hdf5::Shape const shape{array.shape()};
            hdf5::Hyperslab hyperslab(shape);
            hdf5::Shape slice_shape{shape};
            size_t nr_erased_dimensions = 0;

            update_hyperslab_by_slice(
                0, slice, shape, hyperslab, slice_shape, nr_erased_dimensions);

            return read_hyperslab_from_array(array, hyperslab, slice_shape);
        })

        .def("__getitem__", [](
                Array const& array,
                std::vector<py::object> const& indices) -> py::object {

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
            hdf5::Hyperslab hyperslab{shape};

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

            return read_hyperslab_from_array(array, hyperslab, slice_shape);
        })

        // Catch-all for when the argument is not a supported index type
        .def("__getitem__", [](
                Array const& /* array */,
                py::object const& /* index */) -> py::object {

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


        SETITEM_INTEGER(uint8_t)
        SETITEM_INTEGER(int8_t)
        SETITEM_INTEGER(uint16_t)
        SETITEM_INTEGER(int16_t)
        SETITEM_INTEGER(uint32_t)
        SETITEM_INTEGER(int32_t)
        SETITEM_INTEGER(uint64_t)
        SETITEM_INTEGER(int64_t)
        SETITEM_INTEGER(float)
        SETITEM_INTEGER(double)
        // SETITEM_INTEGER(std::string)

#undef SET_ITEM_INTEGER

        SETITEM_SLICE(uint8_t)
        SETITEM_SLICE(int8_t)
        SETITEM_SLICE(uint16_t)
        SETITEM_SLICE(int16_t)
        SETITEM_SLICE(uint32_t)
        SETITEM_SLICE(int32_t)
        SETITEM_SLICE(uint64_t)
        SETITEM_SLICE(int64_t)
        SETITEM_SLICE(float)
        SETITEM_SLICE(double)
        // SETITEM_SLICE(std::string)

#undef SETITEM_SLICE

        SETITEM_INDICES(uint8_t)
        SETITEM_INDICES(int8_t)
        SETITEM_INDICES(uint16_t)
        SETITEM_INDICES(int16_t)
        SETITEM_INDICES(uint32_t)
        SETITEM_INDICES(int32_t)
        SETITEM_INDICES(uint64_t)
        SETITEM_INDICES(int64_t)
        SETITEM_INDICES(float)
        SETITEM_INDICES(double)
        // SETITEM_INDICES(std::string)

#undef SETITEM_INDICES

        .def("__setitem__", [](
            Array& array,
            py::slice const& slice,
            py::array& values)
        {
            auto const info = values.request();

            // Verify values are strings
            // s : ascii chars or utf8 encoded unicode string
            // w : unicode - 4 bytes per code point (and on windows 2?!)

            assert(!info.format.empty());
            bool utf8_encoded_strings = info.format.back() == 's';
            bool unicode_strings = info.format.back() == 'w';

            if(!utf8_encoded_strings && !unicode_strings) {
                throw std::runtime_error(fmt::format(
                    "Expected array with utf8 encoded strings or "
                    "Unicode strings, but got array with format {}",
                    info.format));
            }

            hdf5::Shape const shape{array.shape()};
            hdf5::Hyperslab hyperslab{shape};
            hdf5::Shape slice_shape{shape};
            size_t nr_erased_dimensions = 0;

            update_hyperslab_by_slice(
                0, slice, shape, hyperslab, slice_shape, nr_erased_dimensions);

            if(slice_shape.size() != 1) {
                throw std::runtime_error(
                    "Currently, only 1-D string arrays are supported");
            }

            auto const nr_strings = slice_shape[0];

            // Copy strings to new buffer that is compatible with HDF5
            // variable length strings
            auto hdf5_buffer = std::make_unique<char*[]>(nr_strings);

            if(utf8_encoded_strings) {
                // ASCII and UTF8 encoded strings

                // Length in bytes of each individual string
                std::size_t const nr_bytes_utf8 = info.itemsize;
                assert(nr_bytes_utf8 == std::stoul(info.format));

                // Buffer is a 1-D array with all strings back to back. The
                // maximum length of each string is the same:
                // nr_bytes_utf8. Shorter strings are padded with nulls.
                char const* py_buffer = static_cast<char const*>(info.ptr);

                for(std::size_t i = 0; i < nr_strings; ++i) {

                    // Point it at start of Python string in buffer
                    auto const it = py_buffer + (i * nr_bytes_utf8);

                    // Copy bytes and trim tailing null characters
                    std::string utf8_string{it, nr_bytes_utf8};
                    boost::algorithm::trim_right_if(utf8_string, [](char c)
                        {
                            return c == '\0';
                        });

                    // Make space for the UTF8 encoded string + null
                    // character. Copy the string and append with a null.
                    hdf5_buffer.get()[i] = new char[utf8_string.size() + 1];
                    auto hdf5_string = hdf5_buffer.get()[i];
                    std::copy(
                        utf8_string.begin(), utf8_string.end(), hdf5_string);
                    hdf5_string[utf8_string.size()] = '\0';
                }
            }
            else {
                // Unicode strings

                // Length in bytes of each individual string
                std::size_t const nr_bytes_unicode = info.itemsize;

                // Number of bytes used to represent a code point
                std::size_t const nr_bytes_per_code_point = 4;
                assert(nr_bytes_unicode % nr_bytes_per_code_point == 0);

                // Length in code points of each individual string
                std::size_t const nr_code_points =
                    nr_bytes_unicode / nr_bytes_per_code_point;
                assert(nr_code_points == std::stoul(info.format));

                // assert(nr_bytes_unicode % sizeof(Py_UNICODE) == 0);
                // auto const max_nr_ordinals_string =
                //     nr_bytes_unicode / sizeof(Py_UNICODE);

                // Buffer is a 1-D array with all strings back to back. The
                // maximum length of each string is the same:
                // nr_code_points. Shorter strings are padded with nulls.
                Py_UNICODE const* py_buffer =
                    static_cast<Py_UNICODE const*>(info.ptr);

                for(std::size_t i = 0; i < nr_strings; ++i) {
                    // Point it at start of Python Unicode string in buffer
                    auto const it = py_buffer + i * nr_code_points;

                    // Encode string in UTF8, which is how strings are stored
                    // in LUE. Remove any trailing null characters.
                    py::str py_utf8_string =
                        py::reinterpret_steal<py::str>(
                            ::PyUnicode_EncodeUTF8(
                                it, nr_code_points, nullptr));
                    std::string utf8_string =
                        py_utf8_string.cast<std::string>();
                    boost::algorithm::trim_right_if(utf8_string, [](char c)
                        {
                            return c == '\0';
                        });

                    // Make space for the UTF8 encoded string + null
                    // character. Copy the string and append with a null.
                    hdf5_buffer.get()[i] = new char[utf8_string.size() + 1];
                    auto hdf5_string = hdf5_buffer.get()[i];
                    std::copy(
                        utf8_string.begin(), utf8_string.end(), hdf5_string);
                    hdf5_string[utf8_string.size()] = '\0';
                }
            }

            array.write(hdf5_buffer.get());

            // TODO Make sure this happens also in case of errors
            for(std::size_t i = 0; i < nr_strings; ++i) {
                delete[] hdf5_buffer.get()[i];
            }
        })

        ;

}

}  // namespace data_model
}  // namespace lue
