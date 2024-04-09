#include "lue/gdal/data_type.hpp"
#include "lue/gdal/configure.hpp"
#include <cassert>


namespace lue::gdal {

    auto as_string(GDALDataType const& data_type) -> std::string
    {
        std::string result{};

        switch (data_type)
        {
            case GDT_Unknown:
            {
                result = "GDT_Unknown";
                break;
            }
            case GDT_Byte:
            {
                result = "GDT_Byte";
                break;
            }
#if LUE_GDAL_SUPPORTS_8BIT_SIGNED_INTEGERS
            case GDT_Int8:
            {
                result = "GDT_Int8";
                break;
            }
#endif
            case GDT_UInt16:
            {
                result = "GDT_UInt16";
                break;
            }
            case GDT_Int16:
            {
                result = "GDT_Int16";
                break;
            }
            case GDT_UInt32:
            {
                result = "GDT_UInt32";
                break;
            }
            case GDT_Int32:
            {
                result = "GDT_Int32";
                break;
            }
#if LUE_GDAL_SUPPORTS_64BIT_INTEGERS
            case GDT_UInt64:
            {
                result = "GDT_UInt64";
                break;
            }
            case GDT_Int64:
            {
                result = "GDT_Int64";
                break;
            }
#endif
            case GDT_Float32:
            {
                result = "GDT_Float32";
                break;
            }
            case GDT_Float64:
            {
                result = "GDT_Float64";
                break;
            }
            case GDT_CInt16:
            {
                result = "GDT_CInt16";
                break;
            }
            case GDT_CInt32:
            {
                result = "GDT_CInt32";
                break;
            }
            case GDT_CFloat32:
            {
                result = "GDT_CFloat32";
                break;
            }
            case GDT_CFloat64:
            {
                result = "GDT_CFloat64";
                break;
            }
            case GDT_TypeCount:
            {
                // This should never happen
                assert(false);
                break;
            }
        }

        return result;
    }

}  // namespace lue::gdal
