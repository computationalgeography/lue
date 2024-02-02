#include "lue/gdal/raster.hpp"
#include <cassert>


namespace lue::gdal {

    Raster::Band::Band(RasterBandPtr band):

        _band{band}

    {
        assert(_band);
    }


    auto Raster::Band::data_type() const -> GDALDataType
    {
        return gdal::data_type(*_band);
    }


    auto Raster::Band::blocks() const -> Blocks
    {
        auto [block_size_y, block_size_x] = block_size(*_band);

        assert(block_size_x >= 0);
        assert(block_size_y >= 0);

        auto [nr_rows, nr_cols] = band_size(*_band);

        assert(nr_rows >= 0);
        assert(nr_cols >= 0);

        // TODO Enlarge given some heuristics:
        // - A whole number of times the actual block size (in each dimension)
        // - Number of cells below some sensible value (1GB?)
        block_size_x *= 10;
        block_size_y *= 10;

        block_size_x = std::min(block_size_x, nr_cols);
        block_size_y = std::min(block_size_y, nr_rows);

        return {{nr_cols, nr_rows}, {block_size_x, block_size_y}};
    }


    auto Raster::Band::read_block(Offset const& block_offset, void* buffer) const -> void
    {
        auto cpl_status = _band->ReadBlock(block_offset[0], block_offset[1], buffer);

        if (cpl_status != CE_None)
        {
            throw std::runtime_error("Cannot read block from GDAL raster band");
        }
    }


    auto Raster::Band::read(void* buffer) const -> void
    {
        int const nr_rows = _band->GetYSize();
        int const nr_cols = _band->GetXSize();
        auto const datatype = this->data_type();

        auto cpl_status = _band->RasterIO(
            GF_Read, 0, 0, nr_cols, nr_rows, buffer, nr_cols, nr_rows, datatype, 0, 0, nullptr);

        if (cpl_status != CE_None)
        {
            throw std::runtime_error("Cannot read from GDAL raster band");
        }
    }


    auto Raster::Band::write_block(Offset const& block_offset, void* buffer) -> void
    {
        auto cpl_status = _band->WriteBlock(block_offset[0], block_offset[1], buffer);

        if (cpl_status != CE_None)
        {
            throw std::runtime_error("Cannot write block to GDAL raster band");
        }
    }


    // template<
    //     typename T>
    // void Raster::Band::write(
    //     hl::Raster::Band& raster_band,
    //     ProgressIndicator& progress_indicator)
    // {
    //     auto const blocks = this->blocks();
    //     std::vector<T> values(blocks.block_size());
    //     size_t nr_valid_cells_x;
    //     size_t nr_valid_cells_y;
    //     size_t current_block = 0;
    //
    //     for(size_t block_y = 0; block_y < blocks.nr_blocks_y();
    //             ++block_y) {
    //         for(size_t block_x = 0; block_x < blocks.nr_blocks_x();
    //                 ++block_x) {
    //
    //             read_block(block_x, block_y, values.data());
    //
    //             std::tie(nr_valid_cells_x, nr_valid_cells_y) =
    //                 blocks.nr_valid_cells(block_x, block_y);
    //
    //             hdf5::Shape const shape = { nr_valid_cells_x * nr_valid_cells_y };
    //             auto const memory_dataspace = hdf5::create_dataspace(shape);
    //
    //             hdf5::Offset offset = {
    //                 block_y * blocks.block_size_y(),
    //                 block_x * blocks.block_size_x()
    //             };
    //             hdf5::Count count = { nr_valid_cells_y, nr_valid_cells_x };
    //
    //             raster_band.write(
    //                 memory_dataspace, hdf5::Hyperslab(offset, count), values.data());
    //
    //             progress_indicator.update_progress(++current_block);
    //         }
    //     }
    // }

    // void Raster::Band::write(
    //     hl::Raster::Band& raster_band,
    //     ProgressIndicator& progress_indicator)
    // {
    //     auto datatype = _band->GetRasterDataType();
    //
    //     switch(datatype) {
    //         case GDT_Byte: {
    //             write<uint8_t>(raster_band, progress_indicator);
    //             break;
    //         }
    //         case GDT_UInt16: {
    //             write<uint16_t>(raster_band, progress_indicator);
    //             break;
    //         }
    //         case GDT_Int16: {
    //             write<int16_t>(raster_band, progress_indicator);
    //             break;
    //         }
    //         case GDT_UInt32: {
    //             write<uint32_t>(raster_band, progress_indicator);
    //             break;
    //         }
    //         case GDT_Int32: {
    //             write<int32_t>(raster_band, progress_indicator);
    //             break;
    //         }
    //         case GDT_Float32: {
    //             write<float>(raster_band, progress_indicator);
    //             break;
    //         }
    //         case GDT_Float64: {
    //             write<double>(raster_band, progress_indicator);
    //             break;
    //         }
    //         default: {
    //             throw std::runtime_error("Unsupported datatype");
    //             break;
    //         }
    //     }
    // }


    Raster::Raster(DatasetPtr dataset_ptr):

        _dataset_ptr{std::move(dataset_ptr)}

    {
        assert(_dataset_ptr);
    }


    Raster::Raster(std::string const& dataset_name):

        _dataset_ptr{open_dataset(dataset_name, GDALAccess::GA_ReadOnly)}

    {
    }


    auto Raster::shape() const -> Shape
    {
        return gdal::shape(*_dataset_ptr);
    }


    auto Raster::geo_transform() const -> GeoTransform
    {
        return gdal::geo_transform(*_dataset_ptr);
    }


    auto Raster::set_geo_transform(GeoTransform const& geo_transform) -> void
    {
        gdal::set_geo_transform(*_dataset_ptr, geo_transform);
    }


    auto Raster::nr_bands() const -> Count
    {
        Count const result{nr_raster_bands(*_dataset_ptr)};

        assert(result >= 0);

        return result;
    }


    auto Raster::band(Count const band_nr) const -> Band
    {
        assert(band_nr > 0);
        assert(band_nr <= nr_bands());

        return Raster::Band{raster_band(*_dataset_ptr, band_nr)};
    }

}  // namespace lue::gdal
