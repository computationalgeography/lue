#include "lue/gdal/raster_band.hpp"


namespace lue::gdal {

    /*!
        @brief      Return the datatype of the elements in the raster band
    */
    auto data_type(GDALRasterBand& band) -> GDALDataType
    {
        return band.GetRasterDataType();
    }


    auto block_size(GDALRasterBand& band) -> Shape
    {
        int size_x;
        int size_y;

        band.GetBlockSize(&size_x, &size_y);

        return {size_y, size_x};
    }


    auto band_size(GDALRasterBand& band) -> Shape
    {
        int size_x = band.GetXSize();
        int size_y = band.GetYSize();

        return {size_y, size_x};
    }

}  // namespace lue::gdal
