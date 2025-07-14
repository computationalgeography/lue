#include "lue/py/data_model/conversion.hpp"
#include <format>
#include <map>
#include <optional>


namespace py = pybind11;

namespace lue {

    hdf5::Datatype numpy_type_to_memory_datatype(py::dtype const& dtype)
    {
        auto const kind = dtype.kind();
        auto const size = dtype.itemsize();  // bytes

        std::optional<hdf5::Datatype> datatype;

        switch (kind)
        {
            case 'i':
            {
                // Signed integer
                switch (size)
                {
                    case 1:
                    {
                        datatype = hdf5::native_int8;
                        break;
                    }
                    case 2:
                    {
                        datatype = hdf5::native_int16;
                        break;
                    }
                    case 4:
                    {
                        datatype = hdf5::native_int32;
                        break;
                    }
                    case 8:
                    {
                        datatype = hdf5::native_int64;
                        break;
                    }
                }

                break;
            }
            case 'u':
            {
                // Unsigned integer
                switch (size)
                {
                    case 1:
                    {
                        datatype = hdf5::native_uint8;
                        break;
                    }
                    case 2:
                    {
                        datatype = hdf5::native_uint16;
                        break;
                    }
                    case 4:
                    {
                        datatype = hdf5::native_uint32;
                        break;
                    }
                    case 8:
                    {
                        datatype = hdf5::native_uint64;
                        break;
                    }
                }

                break;
            }
            case 'f':
            {
                // Floating-point
                switch (size)
                {
                    case 4:
                    {
                        datatype = hdf5::native_float32;
                        break;
                    }
                    case 8:
                    {
                        datatype = hdf5::native_float64;
                        break;
                    }
                }

                break;
            }
            case 'U':
            {
                // Unicode string
                datatype = hdf5::create_string_datatype();
                break;
            }
        }

        if (!datatype)
        {
            throw std::runtime_error(std::format("Unsupported dtype (kind={}, itemsize={})", kind, size));
        }

        return *datatype;
    }


    py::dtype hdf5_type_id_to_numpy_dtype(hdf5::Datatype const datatype)
    {
        assert(datatype.is_native());

        py::dtype dtype;

        static std::map<hdf5::Datatype, py::dtype, hdf5::CompareDatatypes> dtype_by_datatype{
            {hdf5::native_uint8, py::dtype::of<std::uint8_t>()},
            {hdf5::native_uint16, py::dtype::of<std::uint16_t>()},
            {hdf5::native_uint32, py::dtype::of<std::uint32_t>()},
            {hdf5::native_uint64, py::dtype::of<std::uint64_t>()},
            {hdf5::native_int8, py::dtype::of<std::int8_t>()},
            {hdf5::native_int16, py::dtype::of<std::int16_t>()},
            {hdf5::native_int32, py::dtype::of<std::int32_t>()},
            {hdf5::native_int64, py::dtype::of<std::int64_t>()},
            {hdf5::native_float32, py::dtype::of<float>()},
            {hdf5::native_float64, py::dtype::of<double>()}};

        if (dtype_by_datatype.find(datatype) == dtype_by_datatype.end())
        {
            throw std::runtime_error(
                std::format("Unsupported datatype ({})", native_datatype_as_string(datatype)));
        }

        return dtype_by_datatype[datatype];
    }


    hdf5::Shape tuple_to_shape(py::tuple const& tuple)
    {
        hdf5::Shape shape(tuple.size());

        for (size_t i = 0; i < tuple.size(); ++i)
        {
            shape[i] = py::int_(tuple[i]);
        }

        return shape;
    }

}  // namespace lue
