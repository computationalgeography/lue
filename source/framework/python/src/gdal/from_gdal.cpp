#include "shape.hpp"
#include "lue/framework/io/gdal.hpp"
#include "lue/framework/io/raster.hpp"
#include <fmt/format.h>
#include <pybind11/pybind11.h>


using namespace pybind11::literals;


namespace lue::framework {
    namespace {

        template<typename Element>
        pybind11::object from_gdal2(std::string const& name, DynamicShape const& partition_shape)
        {
            if (partition_shape.size() != 2)
            {
                throw std::runtime_error(
                    fmt::format("Rank of partition shape must be 2, not {}", partition_shape.size()));
            }

            return pybind11::cast(read<Element>(name, dynamic_shape_to_static_shape<2>(partition_shape)));
        }


        template<typename Element>
        pybind11::object from_gdal2(std::string const& name, pybind11::tuple const& partition_shape)
        {
            return from_gdal2<Element>(name, tuple_to_shape(partition_shape));
        }


        pybind11::object from_gdal(std::string const& name, pybind11::tuple const& partition_shape)
        {
            GDALDataType const data_type{lue::data_type(name)};

            pybind11::object result;

            if (data_type == GDT_Byte)
            {
                result = from_gdal2<uint8_t>(name, partition_shape);
            }
            else if (data_type == GDT_UInt32)
            {
                result = from_gdal2<uint32_t>(name, partition_shape);
            }
            else if (data_type == GDT_Int32)
            {
                result = from_gdal2<int32_t>(name, partition_shape);
            }
#if GDAL_VERSION_NUM >= GDAL_COMPUTE_VERSION(3, 5, 0)
            else if (data_type == GDT_UInt64)
            {
                result = from_gdal2<uint64_t>(name, partition_shape);
            }
            else if (data_type == GDT_Int64)
            {
                result = from_gdal2<int64_t>(name, partition_shape);
            }
#endif
            else if (data_type == GDT_Float32)
            {
                result = from_gdal2<float>(name, partition_shape);
            }
            else if (data_type == GDT_Float64)
            {
                result = from_gdal2<double>(name, partition_shape);
            }

            if (!result)
            {
                throw std::runtime_error(
                    fmt::format("Unsupported datatype: {}", "TODO"));  // as_string(data_type)));
            }

            return result;
        }

    }  // Anonymous namespace


    void bind_from_gdal(pybind11::module& module)
    {
        module.def("from_gdal", from_gdal, "name"_a, "partition_shape"_a);
    }

}  // namespace lue::framework
