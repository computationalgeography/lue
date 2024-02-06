#include "lue/gdal/raster.hpp"
#include <cassert>


namespace lue::gdal {

    Raster::Band::Band(RasterBandPtr band):

        _band_ptr{band}

    {
        assert(_band_ptr);
    }


    /*!
        @sa         gdal::data_type
    */
    auto Raster::Band::data_type() const -> GDALDataType
    {
        return gdal::data_type(*_band_ptr);
    }


    /*!
        @sa         gdal::shape(GDALRasterBand const&)
    */
    auto Raster::Band::size() const -> Shape
    {
        return gdal::band_size(*_band_ptr);
    }


    /*!
        @sa         gdal::block_size
    */
    auto Raster::Band::block_size() const -> Shape
    {
        return gdal::block_size(*_band_ptr);
    }


    /*!
        @sa         gdal::read_block
    */
    auto Raster::Band::read_block(Offset const& block_offset, void* buffer) -> void
    {
        gdal::read_block(*_band_ptr, block_offset, buffer);
    }


    /*!
        @sa         gdal::read
    */
    auto Raster::Band::read(void* buffer) -> void
    {
        gdal::read(*_band_ptr, size(), data_type(), buffer);
    }


    /*!
        @sa         gdal::write
    */
    auto Raster::Band::write(void* buffer) -> void
    {
        gdal::write(*_band_ptr, size(), data_type(), buffer);
    }


    /*!
        @sa         gdal::write_block
    */
    auto Raster::Band::write_block(Offset const& block_offset, void* buffer) -> void
    {
        gdal::write_block(*_band_ptr, block_offset, buffer);
    }


    auto Raster::Band::write(
        Offset const& offset, Shape const& shape, GDALDataType const data_type, void* buffer) -> void
    {
        gdal::write(*_band_ptr, offset, shape, data_type, buffer);
    }


    auto Raster::Band::write(Shape const& shape, GDALDataType const data_type, void* buffer) -> void
    {
        gdal::write(*_band_ptr, shape, data_type, buffer);
    }


    Raster::Raster(DatasetPtr dataset_ptr):

        _dataset_ptr{std::move(dataset_ptr)}

    {
        assert(_dataset_ptr);
    }


    /*!
        @sa         gdal::shape(GDALDataset const&)
    */
    auto Raster::shape() const -> Shape
    {
        return gdal::shape(*_dataset_ptr);
    }


    /*!
        @sa         gdal::geo_transform
    */
    auto Raster::geo_transform() const -> GeoTransform
    {
        return gdal::geo_transform(*_dataset_ptr);
    }


    /*!
        @sa         gdal::set_geo_transform
    */
    auto Raster::set_geo_transform(GeoTransform const& geo_transform) -> void
    {
        gdal::set_geo_transform(*_dataset_ptr, geo_transform);
    }


    /*!
        @sa         gdal::nr_raster_bands
    */
    auto Raster::nr_bands() const -> Count
    {
        return nr_raster_bands(*_dataset_ptr);
    }


    /*!
        @sa         gdal::raster_band
    */
    auto Raster::band(Count const band_nr) const -> Band
    {
        return Raster::Band{raster_band(*_dataset_ptr, band_nr)};
    }

}  // namespace lue::gdal
