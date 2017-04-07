#include "lue_translate/format/gdal.h"
#include "lue_translate/format/gdal_block.h"
#include "lue_utility/progress_indicator.h"
#include "luehl/raster.h"
#include <boost/filesystem.hpp>
#include <iostream>


namespace lue {
namespace utility {
namespace {

hdf5::Datatype gdal_datatype_to_hdf5_datatype(
    GDALDataType const datatype)
{
    hid_t type_id = -1;

    switch(datatype) {
        case GDT_Byte: {
            type_id = H5T_NATIVE_UINT8;
            break;
        }
        case GDT_UInt16: {
            type_id = H5T_NATIVE_UINT16;
            break;
        }
        case GDT_Int16: {
            type_id = H5T_NATIVE_INT16;
            break;
        }
        case GDT_UInt32: {
            type_id = H5T_NATIVE_UINT32;
            break;
        }
        case GDT_Int32: {
            type_id = H5T_NATIVE_INT32;
            break;
        }
        case GDT_Float32: {
            type_id = H5T_NATIVE_FLOAT;
            break;
        }
        case GDT_Float64: {
            type_id = H5T_NATIVE_DOUBLE;
            break;
        }
        default: {
            assert(false);
        }
    }

    assert(type_id >= 0);

    return hdf5::Datatype(type_id);
}


// class GDALDomain
// {
// 
// public:
// 
//     GDALDomain(
//         std::string const& coordinate_reference_system)
// 
//         : coordinate_reference_system(coordinate_reference_system)
// 
//     {
//     }
// 
//     virtual ~GDALDomain()=default;
// 
//     std::string const coordinate_reference_system;
// 
// };
// 
// 
// class GDALRasterDomain:
//     public GDALDomain
// {
// 
// public:
// 
//     GDALRasterDomain(
//         std::string const& coordinate_reference_system,
//         // int const nr_bands,
//         double const west,
//         double const north,
//         double const east,
//         double const south,
//         int const nr_rows,
//         int const nr_cols)
// 
//         : GDALDomain(coordinate_reference_system),
//           // nr_bands{nr_bands},
//           west{west},
//           north{north},
//           east{east},
//           south{south},
//           nr_rows{nr_rows},
//           nr_cols{nr_cols}
// 
//     {
//     }
// 
//     // int nr_bands;
// 
//     double const west;
//     double const north;
//     double const east;
//     double const south;
// 
//     int const nr_rows;
//     int const nr_cols;
// 
// };
// 
// GDALRasterDomain gdal_raster_domain(
//     GDALDataset& dataset)
// {
//     int const nr_rows = dataset.GetRasterYSize();
//     assert(nr_rows > 0);
//     int const nr_cols = dataset.GetRasterXSize();
//     assert(nr_cols > 0);
//     // int const nr_bands = dataset.GetRasterCount();
//     // assert(nr_bands >= 0);
//     // // assert(nr_bands == dataset.GetLayerCount());
//     std::string const coordinate_reference_system = dataset.GetProjectionRef();
// 
//     double geo_transform[6];
//     dataset.GetGeoTransform(geo_transform);
// 
//     double const west = geo_transform[0];
//     double const north = geo_transform[3];
//     double const cell_width = geo_transform[1];
//     assert(cell_width > 0.0);
//     double const cell_height = std::abs(geo_transform[5]);
//     assert(cell_height > 0.0);
//     double const east = west + (nr_cols * cell_width);
//     double const south = north - (nr_rows * cell_height);
// 
//     return GDALRasterDomain(coordinate_reference_system, /* nr_bands, */
//         west, north, east, south,
//         nr_rows, nr_cols);
// }


hl::Raster::Discretization gdal_discretization(
    GDALDataset& dataset)
{
    int const nr_rows = dataset.GetRasterYSize();
    assert(nr_rows > 0);
    int const nr_cols = dataset.GetRasterXSize();
    assert(nr_cols > 0);

    return hl::Raster::Discretization(
        static_cast<size_t>(nr_rows), static_cast<size_t>(nr_cols));
}


hl::Raster::Domain gdal_domain(
    GDALDataset& dataset,
    hl::Raster::Discretization const& discretization)
{
    std::string const crs = dataset.GetProjectionRef();

    double geo_transform[6];
    dataset.GetGeoTransform(geo_transform);

    double const west = geo_transform[0];
    double const north = geo_transform[3];
    double const cell_width = geo_transform[1];
    assert(cell_width > 0.0);
    double const cell_height = std::abs(geo_transform[5]);
    assert(cell_height > 0.0);
    double const east = west + (discretization.nr_cols() * cell_width);
    double const south = north - (discretization.nr_rows() * cell_height);

    return hl::Raster::Domain(crs, west, north, east, south);
}


template<
    typename T>
void write_band(
    GDALRasterBand& gdal_raster_band,
    GDALBlock const& blocks,
    // TODO Make this a band, not a raster
    hl::Raster& /* raster */)
{
    ProgressIndicator progress_indicator(std::cout,
        "Copying blocks", blocks.nr_blocks());

    std::vector<T> values(blocks.block_size());
    size_t nr_valid_cells_x;
    size_t nr_valid_cells_y;
    size_t current_block = 0;

    for(size_t block_y = 0; block_y < blocks.nr_blocks_y();
            ++block_y) {
        for(size_t block_x = 0; block_x < blocks.nr_blocks_x();
                ++block_x) {
            auto cpl_status = gdal_raster_band.ReadBlock(block_x, block_y,
                values.data());
            assert(cpl_status == CE_None);  // TODO

            std::tie(nr_valid_cells_x, nr_valid_cells_y) =
                blocks.nr_valid_cells(block_x, block_y);

            hdf5::Shape const shape = { nr_valid_cells_x * nr_valid_cells_y };
            auto const memory_dataspace = hdf5::create_dataspace(shape);

            hdf5::Offset offset = {
                block_y * blocks.block_size_y(),
                block_x * blocks.block_size_x()
            };
            hdf5::Stride stride = { 1, 1 };
            hdf5::Count count = { nr_valid_cells_y, nr_valid_cells_x };

            // TODO Write the block to the band, not to the raster.
            // raster.write(memory_dataspace, offset, stride, count,
            //     values.data());

            progress_indicator.update_progress(++current_block);
        }
    }
}

}  // Anonymous namespace


/*!
    @brief      Try to open a raster dataset read-only
    @return     A pointer to a ::GDALDataset instance if the dataset can be
                opened. Otherwise a pointer containing nullptr.
*/
GDALDatasetPtr try_open_gdal_raster_dataset_for_read(
    std::string const& dataset_name)
{
    CPLPushErrorHandler(CPLQuietErrorHandler);

    auto result = GDALDatasetPtr(
        static_cast<::GDALDataset*>(::GDALOpenEx(dataset_name.c_str(),
            GDAL_OF_READONLY | GDAL_OF_RASTER, nullptr, nullptr, nullptr)),
        GDALDatasetDeleter{});

    CPLPopErrorHandler();

    return result;
}


/*!
    @brief      Translate a GDAL raster dataset to a LUE dataset
    @param      gdal_dataset GDAL raster dataset to translate
    @param      lue_dataset_name Name of LUE dataset to create
    @exception  .

    In case a LUE dataset named @a lue_dataset_name already exists, it will
    be truncated.
*/
void translate_gdal_raster_dataset_to_lue(
    ::GDALDataset& gdal_dataset,
    // std::string const& gdal_dataset_name,
    std::string const& lue_dataset_name)
{
    std::string const gdal_dataset_pathname = gdal_dataset.GetDescription();
    auto const phenomenon_name =
        boost::filesystem::path(gdal_dataset_pathname).stem().string();
    std::string const property_set_name = "area";

    hl::Raster::Discretization const discretization = gdal_discretization(
        gdal_dataset);
    hl::Raster::Domain const domain = gdal_domain(gdal_dataset,
        discretization);

    auto raster = hl::create_raster(lue_dataset_name, phenomenon_name,
        property_set_name, domain, discretization);


    // Iterate over all bands and write them to the raster.
    int const nr_bands = gdal_dataset.GetRasterCount();
    assert(nr_bands >= 0);

    /// hid_t file_type_id;
    /// hid_t memory_type_id;


    // Import all raster bands.
    for(int b = 1; b <= nr_bands; ++b) {

        GDALRasterBand* gdal_raster_band = gdal_dataset.GetRasterBand(b);
        assert(gdal_raster_band);
        GDALDataType const gdal_datatype =
            gdal_raster_band->GetRasterDataType();
        auto const memory_datatype = gdal_datatype_to_hdf5_datatype(
            gdal_datatype);

        // TODO Add new band
        // raster_band = raster.add_band(band_name, memory_datatype);

        int block_size_x, block_size_y;
        gdal_raster_band->GetBlockSize(&block_size_x, &block_size_y);
        GDALBlock blocks(discretization.nr_cols(), discretization.nr_rows(),
            block_size_x, block_size_y);

        // TODO Write to the band, not to the raster
        switch(gdal_datatype) {
            case GDT_Byte: {
                write_band<uint8_t>(*gdal_raster_band, blocks, raster);
                break;
            }
            case GDT_UInt16: {
                write_band<uint16_t>(*gdal_raster_band, blocks, raster);
                break;
            }
            case GDT_Int16: {
                write_band<int16_t>(*gdal_raster_band, blocks, raster);
                break;
            }
            case GDT_UInt32: {
                write_band<uint32_t>(*gdal_raster_band, blocks, raster);
                break;
            }
            case GDT_Int32: {
                write_band<int32_t>(*gdal_raster_band, blocks, raster);
                break;
            }
            case GDT_Float32: {
                write_band<float>(*gdal_raster_band, blocks, raster);
                break;
            }
            case GDT_Float64: {
                write_band<double>(*gdal_raster_band, blocks, raster);
                break;
            }
            default: {
                assert(false);
                break;
            }
        }
    }


    // TODO Write metadata items
    // TODO Handle various ways of handling no-data in GDAL
}

}  // namespace utility
}  // namespace lue
