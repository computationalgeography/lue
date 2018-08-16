#include "lue/py/conversion.hpp"
#include <numpy/arrayobject.h>
#include <fmt/format.h>
#include <map>


namespace py = pybind11;

namespace lue {

hdf5::Datatype numpy_type_to_memory_datatype(
      py::dtype const& dtype)
{
    auto const kind = dtype.kind();
    auto const size = dtype.itemsize();  // bytes
    hid_t memory_type_id = -1;

    switch(kind) {
        case 'i': {
            // Signed integer
            switch(size) {
                case 1: {
                    memory_type_id = H5T_NATIVE_INT8;
                    break;
                }
                case 2: {
                    memory_type_id = H5T_NATIVE_INT16;
                    break;
                }
                case 4: {
                    memory_type_id = H5T_NATIVE_INT32;
                    break;
                }
                case 8: {
                    memory_type_id = H5T_NATIVE_INT64;
                    break;
                }
            }

            break;
        }
        case 'u': {
            // Unsigned integer
            switch(size) {
                case 1: {
                    memory_type_id = H5T_NATIVE_UINT8;
                    break;
                }
                case 2: {
                    memory_type_id = H5T_NATIVE_UINT16;
                    break;
                }
                case 4: {
                    memory_type_id = H5T_NATIVE_UINT32;
                    break;
                }
                case 8: {
                    memory_type_id = H5T_NATIVE_UINT64;
                    break;
                }
            }

            break;
        }
        case 'f': {
            // Floating-point
            switch(size) {
                case 4: {
                    memory_type_id = H5T_NATIVE_FLOAT;
                    break;
                }
                case 8: {
                    memory_type_id = H5T_NATIVE_DOUBLE;
                    break;
                }
            }

            break;
        }
    }

    if(memory_type_id < 0) {
        throw std::runtime_error(fmt::format(
            "Unsupported dtype (kind={}, itemsize={})", kind, size));
    }

    return hdf5::Datatype{memory_type_id};
}


struct CompareDatatypes
{
    bool operator()(hdf5::Datatype const& lhs, hdf5::Datatype const& rhs) const
    {
        // How to determine whether some data type is less than another one?
        // Potentially, datatype with different addresses represent the
        // same logical type (e.g. when a datatype is copied with H5Tcopy).
        // For now, assume this does not happen. Also, datatype equality
        // is tested using H5Tequal, which 'determines whether two
        // datatype identifiers refer to the same datatype'. Let's use
        // the same semantics.
        return lhs.id().info().addr() < rhs.id().info().addr();
    }
};


py::dtype hdf5_type_id_to_numpy_dtype(
    hdf5::Datatype const datatype)
{
    assert(datatype.is_native());

    py::dtype dtype;

    static hdf5::Datatype const uint8{H5T_NATIVE_UINT8};
    static hdf5::Datatype const uint16{H5T_NATIVE_UINT16};
    static hdf5::Datatype const uint32{H5T_NATIVE_UINT32};
    static hdf5::Datatype const uint64{H5T_NATIVE_UINT64};
    static hdf5::Datatype const int8{H5T_NATIVE_INT8};
    static hdf5::Datatype const int16{H5T_NATIVE_INT16};
    static hdf5::Datatype const int32{H5T_NATIVE_INT32};
    static hdf5::Datatype const int64{H5T_NATIVE_INT64};
    static hdf5::Datatype const float32{H5T_NATIVE_FLOAT};
    static hdf5::Datatype const float64{H5T_NATIVE_DOUBLE};

    // static auto compare_datatypes = [](
    //     hdf5::Datatype const& lhs,
    //     hdf5::Datatype const& rhs)
    // {
    //     return lhs.id().info().addr() < rhs.id().info().addr();
    // };

    // C++20 makes lambda closures default constructable
    // using CompareDatatypes = decltype(compare_datatypes);

    static std::map<hdf5::Datatype, py::dtype, CompareDatatypes>
        dtype_by_datatype
    {
        {uint8, py::dtype::of<std::uint8_t>()},
        {uint16, py::dtype::of<std::uint16_t>()},
        {uint32, py::dtype::of<std::uint32_t>()},
        {uint64, py::dtype::of<std::uint64_t>()},
        {int8, py::dtype::of<std::int8_t>()},
        {int16, py::dtype::of<std::int16_t>()},
        {int32, py::dtype::of<std::int32_t>()},
        {int64, py::dtype::of<std::int64_t>()},
        {float32, py::dtype::of<float>()},
        {float64, py::dtype::of<double>()}
    };

    if(dtype_by_datatype.find(datatype) == dtype_by_datatype.end()) {
        throw std::runtime_error(fmt::format(
            "Unsupported datatype ({})",
            native_datatype_as_string(datatype))
        );
    }

    return dtype_by_datatype[datatype];
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
