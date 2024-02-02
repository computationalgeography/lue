#include "lue/translate/format/gdal_raster.hpp"
#include "lue/data_model/hl.hpp"
#include <array>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <filesystem>


namespace lue::utility {
    namespace {

        // hl::RasterDomain raster_domain(GDALDataset& dataset, hl::RasterDiscretization const&
        // discretization)
        // {
        //     std::string const crs = dataset.GetProjectionRef();

        //     std::array<double, 6> geo_transform{};
        //     dataset.GetGeoTransform(geo_transform.data());

        //     double const west = geo_transform[0];
        //     double const north = geo_transform[3];
        //     double const cell_width = geo_transform[1];
        //     assert(cell_width > 0.0);
        //     double const cell_height = std::abs(geo_transform[5]);
        //     assert(cell_height > 0.0);
        //     double const east = west + (discretization.nr_cols() * cell_width);
        //     double const south = north - (discretization.nr_rows() * cell_height);

        //     return hl::RasterDomain(crs, west, south, east, north);
        // }


        /// hl::RasterDiscretization raster_discretization(GDALDataset& dataset)
        /// {
        ///     int const nr_rows = dataset.GetRasterYSize();
        ///     assert(nr_rows > 0);
        ///     int const nr_cols = dataset.GetRasterXSize();
        ///     assert(nr_cols > 0);

        ///     return hl::RasterDiscretization(
        ///         static_cast<std::size_t>(nr_rows), static_cast<std::size_t>(nr_cols));
        /// }

    }  // Anonymous namespace


    ///     hdf5::Datatype gdal_datatype_to_memory_datatype(GDALDataType const datatype)
    ///     {
    ///         hid_t type_id = -1;
    ///
    ///         switch (datatype)
    ///         {
    ///             case GDT_Byte:
    ///             {
    ///                 type_id = H5T_NATIVE_UINT8;
    ///                 break;
    ///             }
    ///             case GDT_UInt16:
    ///             {
    ///                 type_id = H5T_NATIVE_UINT16;
    ///                 break;
    ///             }
    ///             case GDT_Int16:
    ///             {
    ///                 type_id = H5T_NATIVE_INT16;
    ///                 break;
    ///             }
    ///             case GDT_UInt32:
    ///             {
    ///                 type_id = H5T_NATIVE_UINT32;
    ///                 break;
    ///             }
    ///             case GDT_Int32:
    ///             {
    ///                 type_id = H5T_NATIVE_INT32;
    ///                 break;
    ///             }
    /// #if GDAL_VERSION_NUM >= GDAL_COMPUTE_VERSION(3, 5, 0)
    ///             case GDT_UInt64:
    ///             {
    ///                 type_id = H5T_NATIVE_UINT64;
    ///                 break;
    ///             }
    ///             case GDT_Int64:
    ///             {
    ///                 type_id = H5T_NATIVE_INT64;
    ///                 break;
    ///             }
    /// #endif
    ///             case GDT_Float32:
    ///             {
    ///                 type_id = H5T_NATIVE_FLOAT;
    ///                 break;
    ///             }
    ///             case GDT_Float64:
    ///             {
    ///                 type_id = H5T_NATIVE_DOUBLE;
    ///                 break;
    ///             }
    ///             default:
    ///             {
    ///                 assert(false);
    ///             }
    ///         }
    ///
    ///         assert(type_id >= 0);
    ///
    ///         return hdf5::Datatype(type_id);
    ///     }


    ///     template<
    ///         typename RasterView,
    ///         typename T>  // In-file GDAL element type
    ///     void write(GDALRaster::Band& gdal_raster_band, typename RasterView::Layer& lue_raster_layer)
    ///     {
    ///         auto const blocks = gdal_raster_band.blocks();
    ///         std::vector<T> values(blocks.block_size());
    ///         size_t nr_valid_cells_x;
    ///         size_t nr_valid_cells_y;
    ///
    ///         for (size_t block_y = 0; block_y < blocks.nr_blocks_y(); ++block_y)
    ///         {
    ///             for (size_t block_x = 0; block_x < blocks.nr_blocks_x(); ++block_x)
    ///             {
    ///                 gdal_raster_band.read_block(block_x, block_y, values.data());
    ///
    ///                 std::tie(nr_valid_cells_x, nr_valid_cells_y) = blocks.nr_valid_cells(block_x,
    ///                 block_y);
    ///
    ///                 hdf5::Shape const shape = {nr_valid_cells_x * nr_valid_cells_y};
    ///                 auto memory_dataspace = hdf5::create_dataspace(shape);
    ///
    ///                 hdf5::Offset offset{block_y * blocks.block_size_y(), block_x * blocks.block_size_x()};
    ///                 hdf5::Count count{nr_valid_cells_y, nr_valid_cells_x};
    ///                 hdf5::Hyperslab hyperslab{std::move(offset), std::move(count)};
    ///
    ///                 lue_raster_layer.write(std::move(memory_dataspace), std::move(hyperslab),
    ///                 values.data());
    ///             }
    ///         }
    ///     }
    ///
    ///
    ///     template<typename RasterView>
    ///     void write(GDALRaster::Band& gdal_raster_band, typename RasterView::Layer& lue_raster_layer)
    ///     {
    ///         GDALDataType const datatype{gdal_raster_band.gdal_datatype()};
    ///
    ///         switch (datatype)
    ///         {
    ///             case GDT_Byte:
    ///             {
    ///                 write<RasterView, uint8_t>(gdal_raster_band, lue_raster_layer);
    ///                 break;
    ///             }
    ///             case GDT_UInt16:
    ///             {
    ///                 write<RasterView, uint16_t>(gdal_raster_band, lue_raster_layer);
    ///                 break;
    ///             }
    ///             case GDT_Int16:
    ///             {
    ///                 write<RasterView, int16_t>(gdal_raster_band, lue_raster_layer);
    ///                 break;
    ///             }
    ///             case GDT_UInt32:
    ///             {
    ///                 write<RasterView, uint32_t>(gdal_raster_band, lue_raster_layer);
    ///                 break;
    ///             }
    ///             case GDT_Int32:
    ///             {
    ///                 write<RasterView, int32_t>(gdal_raster_band, lue_raster_layer);
    ///                 break;
    ///             }
    /// #if GDAL_VERSION_NUM >= GDAL_COMPUTE_VERSION(3, 5, 0)
    ///             case GDT_UInt64:
    ///             {
    ///                 write<RasterView, uint64_t>(gdal_raster_band, lue_raster_layer);
    ///                 break;
    ///             }
    ///             case GDT_Int64:
    ///             {
    ///                 write<RasterView, int64_t>(gdal_raster_band, lue_raster_layer);
    ///                 break;
    ///             }
    /// #endif
    ///             case GDT_Float32:
    ///             {
    ///                 write<RasterView, float>(gdal_raster_band, lue_raster_layer);
    ///                 break;
    ///             }
    ///             case GDT_Float64:
    ///             {
    ///                 write<RasterView, double>(gdal_raster_band, lue_raster_layer);
    ///                 break;
    ///             }
    ///             default:
    ///             {
    ///                 throw std::runtime_error("Unsupported GDAL datatype");
    ///                 break;
    ///             }
    ///         }
    ///     }


    /// /*!
    ///     @brief      Try to open a raster dataset read-only
    ///     @return     A pointer to a ::GDALDataset instance if the dataset can be
    ///                 opened. Otherwise a pointer containing nullptr.
    /// */
    /// GDALDatasetPtr try_open_gdal_raster_dataset_for_read(std::string const& dataset_name)
    /// {
    ///     CPLPushErrorHandler(CPLQuietErrorHandler);

    ///     auto result = GDALDatasetPtr(
    ///         static_cast<::GDALDataset*>(::GDALOpenEx(
    ///             dataset_name.c_str(),
    ///             // NOLINTNEXTLINE(hicpp-signed-bitwise)
    ///             GDAL_OF_READONLY | GDAL_OF_RASTER,
    ///             nullptr,
    ///             nullptr,
    ///             nullptr)),
    ///         GDALDatasetDeleter{});

    ///     CPLPopErrorHandler();

    ///     return result;
    /// }


    /// GDALDatasetPtr open_gdal_raster_dataset_for_read(std::string const& dataset_name)
    /// {
    ///     auto result = try_open_gdal_raster_dataset_for_read(dataset_name);

    ///     if (!result)
    ///     {
    ///         throw std::runtime_error("Cannot open raster " + dataset_name);
    ///     }

    ///     return result;
    /// }


    void translate_gdal_raster_dataset_to_lue(
        std::vector<std::string> const& gdal_dataset_names,
        std::string const& lue_dataset_name,
        bool const add,
        Metadata const& /* metadata */)
    {
        namespace lh5 = lue::hdf5;
        namespace ldm = lue::data_model;

        // Create / open dataset
        auto create_dataset = [lue_dataset_name]() { return ldm::create_dataset(lue_dataset_name); };

        auto open_dataset = [lue_dataset_name, add]()
        {
            if (!add)
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
            gdal_raster.domain().west(),
            gdal_raster.domain().south(),
            gdal_raster.domain().east(),
            gdal_raster.domain().north()};

        // TODO For now, we use default names. These could also be obtained
        //     from the metadata passed in.
        std::string const phenomenon_name{"area"};
        std::string const property_set_name{"raster"};

        auto contains_raster = [dataset, phenomenon_name, property_set_name]()
        {
            return dataset.phenomena().contains(phenomenon_name) &&
                   dataset.phenomena()[phenomenon_name].property_sets().contains(property_set_name) &&
                   ldm::constant::contains_raster(dataset, phenomenon_name, property_set_name);
        };

        RasterView raster_view{
            !contains_raster()
                ? ldm::constant::create_raster_view(
                      &dataset, phenomenon_name, property_set_name, grid_shape, space_box)
                : ldm::constant::open_raster_view(&dataset, phenomenon_name, property_set_name)};

        // Add raster layers from input raster(s)
        using RasterLayer = RasterView::Layer;

        for (std::string const& gdal_dataset_name : gdal_dataset_names)
        {
            GDALRaster const gdal_raster{gdal_dataset_name};

            std::string const raster_layer_name{std::filesystem::path(gdal_dataset_name).stem().string()};

            auto band_name = [raster_layer_name,
                              nr_bands = gdal_raster.nr_bands()](int const band_idx) -> std::string {
                return nr_bands == 1 ? raster_layer_name
                                     : fmt::format("{}-{}", raster_layer_name, band_idx + 1);
            };

            for (std::size_t band_idx = 0; band_idx < gdal_raster.nr_bands(); ++band_idx)
            {
                GDALRaster::Band gdal_raster_band{gdal_raster.band(band_idx + 1)};

                RasterLayer lue_raster_layer{
                    raster_view.add_layer(band_name(band_idx), gdal_raster_band.datatype())};

                write<RasterView>(gdal_raster_band, lue_raster_layer);
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

}  // namespace lue::utility
