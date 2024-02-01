#include "lue/gdal/raster_band.hpp"


namespace lue::gdal {

    /*!
        @brief      Return the datatype of the elements in the raster band
    */
    auto data_type(GDALRasterBand& band) -> GDALDataType
    {
        return band.GetRasterDataType();
    }

}  // namespace lue::gdal
