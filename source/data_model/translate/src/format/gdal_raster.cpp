#include "lue/translate/format/gdal_raster.hpp"
#include "lue/data_model/hl.hpp"
#include <boost/filesystem.hpp>
#include <array>
#include <cassert>
#include <cmath>
#include <cstdint>


namespace lue {
namespace utility {
namespace {

hl::RasterDomain raster_domain(
    GDALDataset& dataset,
    hl::RasterDiscretization const& discretization)
{
    std::string const crs = dataset.GetProjectionRef();

    std::array<double, 6> geo_transform{};
    dataset.GetGeoTransform(geo_transform.data());

    double const west = geo_transform[0];
    double const north = geo_transform[3];
    double const cell_width = geo_transform[1];
    assert(cell_width > 0.0);
    double const cell_height = std::abs(geo_transform[5]);
    assert(cell_height > 0.0);
    double const east = west + (discretization.nr_cols() * cell_width);
    double const south = north - (discretization.nr_rows() * cell_height);

    return hl::RasterDomain(crs, west, south, east, north);
}


hl::RasterDiscretization raster_discretization(
    GDALDataset& dataset)
{
    int const nr_rows = dataset.GetRasterYSize();
    assert(nr_rows > 0);
    int const nr_cols = dataset.GetRasterXSize();
    assert(nr_cols > 0);

    return hl::RasterDiscretization(
        static_cast<std::size_t>(nr_rows), static_cast<std::size_t>(nr_cols));
}

}  // Anonymous namespace


hdf5::Datatype gdal_datatype_to_memory_datatype(
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


GDALRaster::Band::Band(
    GDALRasterBand* band)

    : _band{band}

{
    assert(_band);
}


GDALDataType GDALRaster::Band::gdal_datatype() const
{
    return _band->GetRasterDataType();
}


hdf5::Datatype GDALRaster::Band::datatype() const
{
    return gdal_datatype_to_memory_datatype(gdal_datatype());
}


GDALBlock GDALRaster::Band::blocks() const
{
    int block_size_x{};
    int block_size_y{};

    _band->GetBlockSize(&block_size_x, &block_size_y);

    assert(block_size_x >= 0);
    assert(block_size_y >= 0);

    int nr_rows = _band->GetYSize();
    int nr_cols = _band->GetXSize();

    return GDALBlock(
        nr_cols,
        nr_rows,
        block_size_x,
        block_size_y);
}


void GDALRaster::Band::read_block(
    std::size_t block_x,
    std::size_t block_y,
    void* buffer)
{
    auto cpl_status = _band->ReadBlock(block_x, block_y, buffer);

    if(cpl_status != CE_None) {
        throw std::runtime_error("Cannot read block from GDAL raster band");
    }
}


void GDALRaster::Band::read(
    void* buffer)
{
    int const nr_rows = _band->GetYSize();
    int const nr_cols = _band->GetXSize();
    auto const datatype = this->gdal_datatype();

    auto cpl_status =
        _band->RasterIO(
            GF_Read, 0, 0, nr_cols, nr_rows,
            buffer, nr_cols, nr_rows,
            datatype, 0, 0, nullptr);

    if(cpl_status != CE_None) {
        throw std::runtime_error("Cannot read from GDAL raster band");
    }
}


// template<
//     typename T>
// void GDALRaster::Band::write(
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
// 
// void GDALRaster::Band::write(
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


GDALRaster::GDALRaster(
    GDALDatasetPtr dataset)

    : _dataset{std::move(dataset)},
      // _dataset_name{"meh"},
      _domain{},
      _discretization{}

{
    init();
}


GDALRaster::GDALRaster(
    std::string const& dataset_name)

    : _dataset{open_gdal_raster_dataset_for_read(dataset_name)},
      // _dataset_name{dataset_name},
      _domain{},
      _discretization{}

{
    // // Open the stack and read the domain and discretization information
    // auto dataset = open_gdal_raster_dataset_for_read(dataset_name);

    init();
}


void GDALRaster::init()
{
    _discretization = raster_discretization(*_dataset);
    _domain = raster_domain(*_dataset, _discretization);
}


hl::RasterDomain const& GDALRaster::domain() const
{
    return _domain;
}


hl::RasterDiscretization const& GDALRaster::discretization() const
{
    return _discretization;
}


std::size_t GDALRaster::nr_bands() const
{
    auto result = _dataset->GetRasterCount();
    assert(result > 0);
    return result;
}


GDALRaster::Band GDALRaster::band(
    int const nr) const
{
    assert(nr > 0);
    assert(nr <= int(nr_bands()));

    return GDALRaster::Band{_dataset->GetRasterBand(nr)};
}


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
            // NOLINTNEXTLINE(hicpp-signed-bitwise)
            GDAL_OF_READONLY | GDAL_OF_RASTER, nullptr, nullptr, nullptr)),
        GDALDatasetDeleter{});

    CPLPopErrorHandler();

    return result;
}


GDALDatasetPtr open_gdal_raster_dataset_for_read(
    std::string const& dataset_name)
{
    auto result = try_open_gdal_raster_dataset_for_read(dataset_name);

    if(!result) {
        throw std::runtime_error("Cannot open raster " + dataset_name);
    }

    return result;
}


void translate_gdal_raster_dataset_to_lue(
    std::vector<std::string> const& gdal_dataset_names,
    std::string const& lue_dataset_name,
    bool const add,
    Metadata const& /* metadata */)
{
    namespace lh5 = lue::hdf5;
    namespace ldm = lue::data_model;

    // Create / open dataset
    auto create_dataset =
        [lue_dataset_name]()
        {
            return ldm::create_dataset(lue_dataset_name);
        };

    auto open_dataset =
        [lue_dataset_name, add]()
        {
            if(!add)
            {
                throw std::runtime_error(fmt::format("Dataset {} already exists", lue_dataset_name));
            }

            return ldm::open_dataset(lue_dataset_name);
        };

    ldm::Dataset dataset{!ldm::dataset_exists(lue_dataset_name) ? create_dataset() : open_dataset()};

    // Each dataset passed in is a GDAL raster dataset. Each of these
    // can contain multiple layers. Layers from rasters with the same
    // domain and discretization can be stored in the same property-set.

    // TODO We are assuming all rasters passed in can be stored in the
    //     same property-set (have the same domain +
    //     discretization). Update this logic if that is not the case.

    assert(!gdal_dataset_names.empty());

    GDALRaster gdal_raster{gdal_dataset_names[0]};

    // Create / open raster view
    using RasterView = data_model::constant::RasterView<data_model::Dataset*>;
    using SpaceBox = RasterView::SpaceBox;

    lh5::Shape const grid_shape{
        gdal_raster.discretization().nr_rows(), gdal_raster.discretization().nr_cols()};
    SpaceBox const space_box{
            gdal_raster.domain().west(), gdal_raster.domain().south(),
            gdal_raster.domain().east(), gdal_raster.domain().north()
        };

    // TODO For now, we use default names. These could also be obtained
    //     from the metadata passed in.
    std::string const phenomenon_name{"area"};
    std::string const property_set_name{"raster"};

    auto contains_raster =
        [dataset, phenomenon_name, property_set_name]()
        {
            return
                dataset.phenomena().contains(phenomenon_name) &&
                dataset.phenomena()[phenomenon_name].property_sets().contains(property_set_name) &&
                ldm::constant::contains_raster(dataset, phenomenon_name, property_set_name);
        };

    RasterView raster_view{
            !contains_raster()
                ? ldm::constant::create_raster_view(
                        &dataset, phenomenon_name, property_set_name, grid_shape, space_box)
                : ldm::constant::open_raster_view(
                        &dataset, phenomenon_name, property_set_name)
        };

    // Add raster layers from input raster(s)
    using RasterLayer = RasterView::Layer;

    for(std::string const& gdal_dataset_name: gdal_dataset_names)
    {
        GDALRaster const gdal_raster{gdal_dataset_name};

        std::string const raster_layer_name{
            boost::filesystem::path(gdal_dataset_name).stem().string()};

        auto band_name =
            [raster_layer_name, nr_bands=gdal_raster.nr_bands()](
                int const band_idx) -> std::string
            {
                return nr_bands == 1
                    ? raster_layer_name
                    : fmt::format("{}-{}", raster_layer_name, band_idx + 1)
                    ;
            };

        for(std::size_t band_idx = 0; band_idx < gdal_raster.nr_bands(); ++band_idx)
        {
            GDALRaster::Band band{gdal_raster.band(band_idx + 1)};

            RasterLayer raster_layer{raster_view.add_layer(band_name(band_idx), band.datatype())};

            std::unique_ptr<uint8_t[]> buffer{new uint8_t[
                band.datatype().size() *
                gdal_raster.discretization().nr_rows() * gdal_raster.discretization().nr_cols()]};

            band.read(buffer.get());
            raster_layer.write(buffer.get());
        }
    }





    // Create data structure that can be converted to a JSON object. This
    // data structure represents the import request.

    // hier verder

    // auto datasets{open_gdal_datasets(gdal_dataset_names)};

    // Scenarios:
    // - Import a bunch of rasters covering the same space box. All
    //     rasters belong to the same object. For each raster a different
    //     property is made.
    // - Import a bunch of rasters covering different space boxes. All
    //     rasters belong to different objects. A single property is made.

    // Figure out whether the space boxes differ
    // auto const space_boxes{space_boxes(datasets)};

    // bool space_boxes_differ


    // // Translate to LUE format using common JSON import
    // translate_json_to_lue(lue_json, lue_dataset_name, add, metadata);
}


}  // namespace utility
}  // namespace lue
