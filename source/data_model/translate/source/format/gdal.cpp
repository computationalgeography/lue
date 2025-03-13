#include "lue/translate/format/gdal.hpp"
#include "lue/data_model/hl.hpp"
#include "lue/translate/stack_name.hpp"


namespace lue::utility {

    namespace {

        auto blocks(gdal::Shape const& band_shape, gdal::Shape const& block_shape) -> gdal::Blocks
        {
            auto [block_size_y, block_size_x] = block_shape;

            assert(block_size_x >= 0);
            assert(block_size_y >= 0);

            auto [nr_rows, nr_cols] = band_shape;

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


        auto hdf5_shape_to_gdal_shape(hdf5::Shape const& hdf5_shape) -> gdal::Shape
        {
            assert(std::size(hdf5_shape) == 2);

            return gdal::Shape{
                static_cast<gdal::Count>(hdf5_shape[0]), static_cast<gdal::Count>(hdf5_shape[1])};
        }

    }  // Anonymous namespace


    auto gdal_data_type_to_memory_data_type(GDALDataType const data_type) -> hdf5::Datatype
    {
        hdf5::Datatype result{};

        switch (data_type)
        {
            case GDT_Byte:
            {
                result = hdf5::native_datatype<std::uint8_t>();
                break;
            }
#if LUE_GDAL_SUPPORTS_8BIT_SIGNED_INTEGERS
            case GDT_Int8:
            {
                result = hdf5::native_datatype<std::int8_t>();
                break;
            }
#endif
            case GDT_UInt16:
            {
                result = hdf5::native_datatype<std::uint16_t>();
                break;
            }
            case GDT_Int16:
            {
                result = hdf5::native_datatype<std::int16_t>();
                break;
            }
            case GDT_UInt32:
            {
                result = hdf5::native_datatype<std::uint32_t>();
                break;
            }
            case GDT_Int32:
            {
                result = hdf5::native_datatype<std::int32_t>();
                break;
            }
#if LUE_GDAL_SUPPORTS_64BIT_INTEGERS
            case GDT_UInt64:
            {
                result = hdf5::native_datatype<std::uint64_t>();
                break;
            }
            case GDT_Int64:
            {
                result = hdf5::native_datatype<std::int64_t>();
                break;
            }
#endif
            case GDT_Float32:
            {
                result = hdf5::native_datatype<float>();
                break;
            }
            case GDT_Float64:
            {
                result = hdf5::native_datatype<double>();
                break;
            }
            default:
            {
                throw std::runtime_error("Cannot map GDAL data type to LUE data type");
            }
        }

        return result;
    }


    auto memory_data_type_to_gdal_data_type(hdf5::Datatype const& data_type) -> GDALDataType
    {
        GDALDataType result{GDT_Unknown};

        if (data_type == hdf5::native_uint8)
        {
            result = gdal::data_type_v<std::uint8_t>;
        }
#if LUE_GDAL_SUPPORTS_8BIT_SIGNED_INTEGERS
        else if (data_type == hdf5::native_int8)
        {
            result = gdal::data_type_v<std::int8_t>;
        }
#endif
        else if (data_type == hdf5::native_uint16)
        {
            result = gdal::data_type_v<std::uint16_t>;
        }
        else if (data_type == hdf5::native_int16)
        {
            result = gdal::data_type_v<std::int16_t>;
        }
        else if (data_type == hdf5::native_uint32)
        {
            result = gdal::data_type_v<std::uint32_t>;
        }
        else if (data_type == hdf5::native_int32)
        {
            result = gdal::data_type_v<std::int32_t>;
        }
#if LUE_GDAL_SUPPORTS_64BIT_INTEGERS
        else if (data_type == hdf5::native_uint64)
        {
            result = gdal::data_type_v<std::uint64_t>;
        }
        else if (data_type == hdf5::native_int64)
        {
            result = gdal::data_type_v<std::int64_t>;
        }
#endif
        else if (data_type == hdf5::native_float32)
        {
            result = gdal::data_type_v<float>;
        }
        else if (data_type == hdf5::native_float64)
        {
            result = gdal::data_type_v<double>;
        }
        else
        {
            throw std::runtime_error("Cannot map LUE data type to GDAL data type");
        }

        return result;
    }


    template<typename T>
    void lue_to_gdal(
        data_model::same_shape::constant_shape::Value const& value,
        data_model::Index const time_point_idx,
        data_model::Index const time_step_idx,
        gdal::Raster::Band& raster_band)
    {
        // It is assumed here that value contains a 2D array for multiple
        // locations in time. The 2D array to write to the raster band is
        // located at the index passed in.

        auto const blocks = utility::blocks(raster_band.shape(), raster_band.block_shape());
        auto const& block_shape = blocks.block_shape();
        std::vector<T> values(gdal::nr_elements(block_shape));

        auto const [nr_blocks_y, nr_blocks_x] = blocks.shape_in_blocks();

        hdf5::Datatype const memory_datatype{hdf5::native_datatype<T>()};

        for (gdal::Count block_y = 0; block_y < nr_blocks_y; ++block_y)
        {
            for (gdal::Count block_x = 0; block_x < nr_blocks_x; ++block_x)
            {
                auto const [nr_valid_cells_x, nr_valid_cells_y] =
                    blocks.shape_in_valid_cells({block_x, block_y});


                // hdf5::Shape const shape = {
                //     nr_valid_cells_x * nr_valid_cells_y
                // };
                // auto const memory_dataspace = hdf5::create_dataspace(shape);

                hdf5::Offset offset{
                    time_point_idx,
                    time_step_idx,
                    static_cast<hdf5::Offset::value_type>(block_y * block_shape[0]),
                    static_cast<hdf5::Offset::value_type>(block_x * block_shape[1])};
                hdf5::Count count{
                    1,
                    1,
                    static_cast<hdf5::Count::value_type>(nr_valid_cells_y),
                    static_cast<hdf5::Count::value_type>(nr_valid_cells_x)};
                hdf5::Hyperslab const hyperslab{offset, count};

                value.read(memory_datatype, hyperslab, values.data());
                raster_band.write_block({block_y, block_x}, values.data());
            }
        }
    }


    void lue_to_gdal(
        data_model::same_shape::constant_shape::Value const& value,
        data_model::Index const time_point_idx,
        data_model::Index const time_step_idx,
        gdal::Raster::Band& raster_band)
    {
        hdf5::Datatype const memory_datatype{gdal_data_type_to_memory_data_type(raster_band.data_type())};

        if (memory_datatype == hdf5::native_uint8)
        {
            lue_to_gdal<std::uint8_t>(value, time_point_idx, time_step_idx, raster_band);
        }
        else if (memory_datatype == hdf5::native_uint16)
        {
            lue_to_gdal<std::uint16_t>(value, time_point_idx, time_step_idx, raster_band);
        }
        else if (memory_datatype == hdf5::native_int16)
        {
            lue_to_gdal<std::int16_t>(value, time_point_idx, time_step_idx, raster_band);
        }
        else if (memory_datatype == hdf5::native_uint32)
        {
            lue_to_gdal<std::uint32_t>(value, time_point_idx, time_step_idx, raster_band);
        }
        else if (memory_datatype == hdf5::native_int32)
        {
            lue_to_gdal<std::int32_t>(value, time_point_idx, time_step_idx, raster_band);
        }
        else if (memory_datatype == hdf5::native_uint64)
        {
            lue_to_gdal<std::uint64_t>(value, time_point_idx, time_step_idx, raster_band);
        }
        else if (memory_datatype == hdf5::native_int64)
        {
            lue_to_gdal<std::int64_t>(value, time_point_idx, time_step_idx, raster_band);
        }
        else if (memory_datatype == hdf5::native_float32)
        {
            lue_to_gdal<float>(value, time_point_idx, time_step_idx, raster_band);
        }
        else if (memory_datatype == hdf5::native_float64)
        {
            lue_to_gdal<double>(value, time_point_idx, time_step_idx, raster_band);
        }
        else
        {
            throw std::runtime_error("Cannot lue_to_gdal value with this datatype to a GDAL raster band");
        }
    }


    template<typename T>
    void lue_to_gdal(data_model::Array const& array, gdal::Raster::Band& raster_band)
    {
        if (array.has_no_data_value())
        {
            raster_band.set_no_data_value(array.no_data_value<T>());
        }

        // It is assumed here that array only contains a 2D array
        auto const blocks = utility::blocks(raster_band.shape(), raster_band.block_shape());
        auto const& block_shape = blocks.block_shape();
        std::vector<T> values(gdal::nr_elements(block_shape));

        auto const [nr_blocks_y, nr_blocks_x] = blocks.shape_in_blocks();

        hdf5::Datatype const memory_datatype{hdf5::native_datatype<T>()};

        for (gdal::Count block_y = 0; block_y < nr_blocks_y; ++block_y)
        {
            for (gdal::Count block_x = 0; block_x < nr_blocks_x; ++block_x)
            {
                auto const [nr_valid_cells_x, nr_valid_cells_y] =
                    blocks.shape_in_valid_cells({block_x, block_y});

                hdf5::Offset offset{
                    static_cast<hdf5::Offset::value_type>(block_y * block_shape[0]),
                    static_cast<hdf5::Offset::value_type>(block_x * block_shape[1])};
                hdf5::Count count{
                    static_cast<hdf5::Count::value_type>(nr_valid_cells_y),
                    static_cast<hdf5::Count::value_type>(nr_valid_cells_x)};
                hdf5::Hyperslab const hyperslab{offset, count};

                array.read(memory_datatype, hyperslab, values.data());
                raster_band.write_block({block_y, block_x}, values.data());
            }
        }
    }


    void lue_to_gdal(data_model::Array const& array, gdal::Raster::Band& raster_band)
    {
        hdf5::Datatype const memory_datatype{gdal_data_type_to_memory_data_type(raster_band.data_type())};

        if (memory_datatype == hdf5::native_uint8)
        {
            lue_to_gdal<std::uint8_t>(array, raster_band);
        }
        else if (memory_datatype == hdf5::native_uint16)
        {
            lue_to_gdal<std::uint16_t>(array, raster_band);
        }
        else if (memory_datatype == hdf5::native_int16)
        {
            lue_to_gdal<std::int16_t>(array, raster_band);
        }
        else if (memory_datatype == hdf5::native_uint32)
        {
            lue_to_gdal<std::uint32_t>(array, raster_band);
        }
        else if (memory_datatype == hdf5::native_int32)
        {
            lue_to_gdal<std::int32_t>(array, raster_band);
        }
        else if (memory_datatype == hdf5::native_uint64)
        {
            lue_to_gdal<std::uint64_t>(array, raster_band);
        }
        else if (memory_datatype == hdf5::native_int64)
        {
            lue_to_gdal<std::int64_t>(array, raster_band);
        }
        else if (memory_datatype == hdf5::native_float32)
        {
            lue_to_gdal<float>(array, raster_band);
        }
        else if (memory_datatype == hdf5::native_float64)
        {
            lue_to_gdal<double>(array, raster_band);
        }
        else
        {
            throw std::runtime_error("Cannot write value with this datatype to a GDAL raster band");
        }
    }


    auto translate_lue_dataset_to_gdal_raster(
        data_model::Dataset& dataset, std::string const& raster_name, Metadata const& metadata) -> void
    {
        // Find information about where to read the raster from: phenomenon/property_set/property
        std::string const dataset_name{std::filesystem::path(raster_name).stem().string()};

        auto const& root_json = metadata.object();
        auto const datasets_json = json::object(root_json, "datasets");
        auto const dataset_json = json::object(datasets_json, "name", dataset_name);
        std::string const phenomenon_name{json::string(dataset_json, "phenomenon")};
        std::string const property_set_name{json::string(dataset_json, "property_set")};
        auto const raster_json = json::object(dataset_json, "raster");
        auto const bands_json = json::object(raster_json, "bands");

        gdal::Count const nr_bands{static_cast<gdal::Count>(bands_json.size())};

        if (nr_bands == 0)
        {
            return;
        }

        std::string const driver_name{gdal::driver_name(raster_name)};

        // If the constant raster view finds a raster with the property name
        // requested, export it to a single GDAL raster
        if (data_model::constant::contains_raster(dataset, phenomenon_name, property_set_name))
        {
            using RasterView = data_model::constant::RasterView<data_model::Dataset*>;
            using RasterLayer = RasterView::Layer;

            RasterView raster_view{&dataset, phenomenon_name, property_set_name};

            auto it = bands_json.begin();
            auto const property_name = json::string(*it, "name");

            if (!raster_view.contains(property_name))
            {
                throw std::runtime_error(std::format(
                    "Constant raster layer named {} is not part of property_set {}",
                    property_name,
                    property_set_name));
            }

            RasterLayer layer{raster_view.layer(property_name)};
            auto const& space_box{raster_view.space_box()};
            gdal::Shape const raster_shape{hdf5_shape_to_gdal_shape(raster_view.grid_shape())};
            auto const [nr_rows, nr_cols] = raster_shape;

            double const west{space_box[0]};
            double const south{space_box[1]};
            double const east{space_box[2]};
            double const north{space_box[3]};

            assert(east >= west);
            assert(north >= south);
            assert(nr_rows > 0);
            assert(nr_cols > 0);

            double const cell_width{(east - west) / nr_cols};
            double const cell_height{(north - south) / nr_rows};

            gdal::GeoTransform geo_transform{west, cell_width, 0, north, 0, -cell_height};

            // TODO
            // OGRSpatialReference oSRS;
            // char *pszSRS_WKT = NULL;
            // GDALRasterBand *poBand;
            // GByte abyRaster[512*512];
            // oSRS.SetUTM( 11, TRUE );
            // oSRS.SetWellKnownGeogCS( "NAD27" );
            // oSRS.exportToWkt( &pszSRS_WKT );
            // poDstDS->SetProjection( pszSRS_WKT );
            // CPLFree( pszSRS_WKT );

            gdal::Raster raster{gdal::create_dataset(
                driver_name,
                raster_name,
                raster_shape,
                nr_bands,
                memory_data_type_to_gdal_data_type(layer.memory_datatype()))};

            raster.set_geo_transform(geo_transform);

            gdal::Count band_nr{1};
            gdal::Raster::Band raster_band{raster.band(band_nr)};
            lue_to_gdal(layer, raster_band);


            // TODO
            // for (; it != bands_json.end(); ++it, ++band_nr)
            // {
            //     auto const property_name = json::string(*it, "name");

            //     if (!raster_view.contains(property_name))
            //     {
            //         throw std::runtime_error(std::format(
            //             "Constant raster layer named {} is not part of property_set {}",
            //             property_name,
            //             property_set_name));
            //     }

            //     gdal::Raster::Band raster_band{raster.band(band_nr)};

            //     lue_to_gdal(layer, raster_band);
            // }
        }
        // If the variable raster view finds a raster layer with the property
        // name requested, export it to a stack of GDAL rasters
        else if (data_model::variable::contains_raster(dataset, phenomenon_name, property_set_name))
        {
            using RasterView = data_model::variable::RasterView<data_model::Dataset*>;
            // using RasterLayer = RasterView::Layer;

            [[maybe_unused]] RasterView raster_view{&dataset, phenomenon_name, property_set_name};

            // TODO
            throw std::runtime_error("Importing stacks not supported yet. WIP.");

            ///     if (!raster_view.contains(property_name))
            ///     {
            ///         throw std::runtime_error(std::format(
            ///             "Variable raster layer named {} is not part of property_set {}",
            ///             property_name,
            ///             property_set_name));
            ///     }

            ///     assert(hdf5::size_of_shape(raster_view.grid_shape()) > 0);

            ///     data_model::Count const nr_bands{1};
            ///     data_model::Index const time_point_idx{0};  // Single time box
            ///     StackName stack_name{raster_name};
            ///     RasterLayer layer{raster_view.layer(property_name)};
            ///     auto const& space_box{raster_view.space_box()};

            ///     for (data_model::Count time_step = 0; time_step < raster_view.nr_time_steps();
            ///     ++time_step)
            ///     {
            ///         gdal::Raster raster{gdal::create_dataset(
            ///             "GTiff",
            ///             stack_name[time_step],
            ///             hdf5_shape_to_gdal_shape(raster_view.grid_shape()),
            ///             nr_bands,
            ///             memory_data_type_to_gdal_data_type(layer.memory_datatype()))};

            ///         // FIXME
            ///         double const cell_size{0.000992063492063};
            ///         double const west{space_box[0]};
            ///         double const north{space_box[3]};
            ///         gdal::GeoTransform geo_transform{west, cell_size, 0, north, 0, -cell_size};

            ///         raster.set_geo_transform(geo_transform);

            ///         // TODO(KDJ)
            ///         // OGRSpatialReference oSRS;
            ///         // char *pszSRS_WKT = NULL;
            ///         // GDALRasterBand *poBand;
            ///         // GByte abyRaster[512*512];
            ///         // oSRS.SetUTM( 11, TRUE );
            ///         // oSRS.SetWellKnownGeogCS( "NAD27" );
            ///         // oSRS.exportToWkt( &pszSRS_WKT );
            ///         // poDstDS->SetProjection( pszSRS_WKT );
            ///         // CPLFree( pszSRS_WKT );

            ///         gdal::Raster::Band raster_band{raster.band(1)};
            ///         lue_to_gdal(layer, time_point_idx, time_step, raster_band);
            ///     }
        }
        else
        {
            throw std::runtime_error(std::format(
                "No property-set named {} found in phenomenon {}", property_set_name, phenomenon_name));
        }


        /// // Figure out which property-sets are selected
        /// auto const& root_json = metadata.object();
        /// std::string const lue_dataset_name = std::filesystem::path(dataset.pathname()).stem().string();

        /// // Dataset -----------------------------------------------------------------
        /// if (!json::has_key(root_json, lue_dataset_name))
        /// {
        ///     throw std::runtime_error(
        ///         std::format("No information for dataset {} present in metadata", lue_dataset_name));
        /// }

        /// auto const dataset_json = json::object(root_json, lue_dataset_name);

        /// // Phenomena -----------------------------------------------------------
        /// if (!json::has_key(dataset_json, "phenomena"))
        /// {
        ///     throw std::runtime_error(std::format(
        ///         "No information about phenomena present in metadata for "
        ///         "dataset {}",
        ///         lue_dataset_name));
        /// }

        /// auto const phenomena_json = json::object(dataset_json, "phenomena");

        /// if (phenomena_json.size() != 1)
        /// {
        ///     throw std::runtime_error(std::format(
        ///         "Expected information about 1 phenomenon in metadata for "
        ///         "dataset {}, but got {}",
        ///         lue_dataset_name,
        ///         phenomena_json.size()));
        /// }

        /// auto const phenomenon_json = phenomena_json.front();
        /// std::string const phenomenon_name = json::string(phenomenon_json, "name");

        /// // Property-sets -----------------------------------------------------------
        /// if (!json::has_key(phenomenon_json, "property_sets"))
        /// {
        ///     throw std::runtime_error(std::format(
        ///         "No information about property-sets present in metadata for "
        ///         "phenomenon {} in dataset {}",
        ///         phenomenon_name,
        ///         lue_dataset_name));
        /// }

        /// auto const property_sets_json = json::object(phenomenon_json, "property_sets");

        /// if (property_sets_json.size() != 1)
        /// {
        ///     throw std::runtime_error(std::format(
        ///         "Expected information about 1 property-set in metadata for "
        ///         "phenomenon {} in dataset {}, but got {}",
        ///         phenomenon_name,
        ///         lue_dataset_name,
        ///         property_sets_json.size()));
        /// }

        /// auto const property_set_json = property_sets_json.front();
        /// std::string const property_set_name = json::string(property_set_json, "name");

        /// // Properties --------------------------------------------------------------
        /// std::string property_name;

        /// if (json::has_key(property_set_json, "properties"))
        /// {
        ///     auto const properties_json = json::object(property_set_json, "properties");

        ///     if (properties_json.size() != 1)
        ///     {
        ///         throw std::runtime_error(std::format(
        ///             "Expected information about 1 property in metadata for "
        ///             "property-set {} in phenomenon {} in dataset {}, but got {}",
        ///             property_set_name,
        ///             phenomenon_name,
        ///             lue_dataset_name,
        ///             properties_json.size()));
        ///     }

        ///     auto const property_json = properties_json.front();
        ///     property_name = json::string(property_json, "name");
        /// }
        /// else
        /// {
        ///     property_name = std::filesystem::path{raster_name}.stem().string();
        /// }

        /// // If the constant raster view finds a raster with the property name
        /// // requested, export it to a single GDAL raster
        /// if (data_model::constant::contains_raster(dataset, phenomenon_name, property_set_name))
        /// {

        ///     using RasterView = data_model::constant::RasterView<data_model::Dataset*>;
        ///     using RasterLayer = RasterView::Layer;

        ///     RasterView raster_view{&dataset, phenomenon_name, property_set_name};

        ///     if (!raster_view.contains(property_name))
        ///     {
        ///         throw std::runtime_error(std::format(
        ///             "Constant raster layer named {} is not part of property_set {}",
        ///             property_name,
        ///             property_set_name));
        ///     }

        ///     assert(hdf5::size_of_shape(raster_view.grid_shape()) > 0);

        ///     data_model::Count const nr_bands{1};
        ///     RasterLayer layer{raster_view.layer(property_name)};
        ///     auto const& space_box{raster_view.space_box()};

        ///     gdal::Raster raster{gdal::create_dataset(
        ///         "GTiff",
        ///         raster_name,
        ///         hdf5_shape_to_gdal_shape(raster_view.grid_shape()),
        ///         nr_bands,
        ///         memory_data_type_to_gdal_data_type(layer.memory_datatype()))};

        ///     // FIXME
        ///     double const cell_size{0.000992063492063};
        ///     double const west{space_box[0]};
        ///     double const north{space_box[3]};
        ///     gdal::GeoTransform geo_transform{west, cell_size, 0, north, 0, -cell_size};

        ///     raster.set_geo_transform(geo_transform);

        ///     // TODO(KDJ)
        ///     // OGRSpatialReference oSRS;
        ///     // char *pszSRS_WKT = NULL;
        ///     // GDALRasterBand *poBand;
        ///     // GByte abyRaster[512*512];
        ///     // oSRS.SetUTM( 11, TRUE );
        ///     // oSRS.SetWellKnownGeogCS( "NAD27" );
        ///     // oSRS.exportToWkt( &pszSRS_WKT );
        ///     // poDstDS->SetProjection( pszSRS_WKT );
        ///     // CPLFree( pszSRS_WKT );

        ///     gdal::Raster::Band raster_band{raster.band(1)};
        ///     lue_to_gdal(layer, raster_band);
        /// }


        /// ///     // Find requested information in LUE dataset -------------------------------
        /// ///     Phenomenon& phenomenon = dataset.phenomena()[phenomenon_name];
        /// ///     PropertySet& property_set = phenomenon.property_sets()[property_set_name];
        /// ///
        /// ///     auto const& object_tracker{property_set.object_tracker()};
        /// ///
        /// ///     // Assert there is only a single active set
        /// ///     if(object_tracker.active_set_index().nr_indices() != 1) {
        /// ///         throw std::runtime_error("Expected a single active set");
        /// ///
        /// ///     }
        /// ///
        /// ///     // Assert the one active set contains only a single active object
        /// ///     if(object_tracker.active_object_id().nr_ids() != 1) {
        /// ///         throw std::runtime_error("Expected a single active object");
        /// ///     }
        /// ///
        /// ///
        /// ///     if(!property_set.has_space_domain()) {
        /// ///         throw std::runtime_error("Property-set does not have a space domain");
        /// ///     }
        /// ///
        /// ///     auto const& space_domain{property_set.space_domain()};
        /// ///
        /// ///     if(space_domain.configuration().value<Mobility>() != Mobility::stationary) {
        /// ///         throw std::runtime_error("Expected stationary space domain items");
        /// ///     }
        /// ///
        /// ///     if(space_domain.configuration().value<SpaceDomainItemType>() !=
        /// ///             SpaceDomainItemType::box) {
        /// ///         throw std::runtime_error("Expected space boxes");
        /// ///     }
        /// ///
        /// ///     StationarySpaceBox const space_domain_items{
        /// ///         const_cast<SpaceDomain&>(space_domain).value<StationarySpaceBox>()};
        /// ///
        /// ///     assert(space_domain_items.nr_boxes() == 1);
        /// ///     assert(space_domain_items.array_shape().size() == 1);
        /// ///     assert(space_domain_items.array_shape()[0] == 4);
        /// ///     assert(space_domain_items.memory_datatype() == hdf5::native_float64);
        /// ///
        /// ///     std::vector<double> coordinates(space_domain_items.array_shape()[0]);
        /// ///     space_domain_items.read(coordinates.data());
        /// ///
        /// ///     Properties& properties{property_set.properties()};
        /// ///
        /// ///     if(!properties.contains(property_name)) {
        /// ///         throw std::runtime_error(std::format(
        /// ///             "Property {} is not part of property-set {}",
        /// ///             property_name, property_set_name));
        /// ///     }
        /// ///
        /// ///     if(properties.shape_per_object(property_name) !=
        /// ///             ShapePerObject::different) {
        /// ///         throw std::runtime_error(
        /// ///             "Only ShapePerObject::different is currently supported");
        /// ///     }
        /// ///
        /// ///     if(properties.value_variability(property_name) !=
        /// ///             ValueVariability::variable) {
        /// ///         throw std::runtime_error(
        /// ///             "Only ValueVariability::variable is currently supported");
        /// ///     }
        /// ///
        /// ///     if(properties.shape_variability(property_name) !=
        /// ///             ShapeVariability::constant) {
        /// ///         throw std::runtime_error(
        /// ///             "Only ShapeVariability::constant is currently supported");
        /// ///     }
        /// ///
        /// ///     using Properties = different_shape::constant_shape::Properties;
        /// ///     using Property = different_shape::constant_shape::Property;
        /// ///
        /// ///     // The values of this property need to be written to a stack of rasters,
        /// ///     // using the GDAL API. Each value is a raster. Each location in time
        /// ///     // is a slice.
        /// ///     Property const& property{properties.collection<Properties>()[property_name]};
        /// ///
        /// ///     // For the current active set:
        /// ///     // - Iterate over each active object in the active set
        /// ///
        /// ///         // For the current object:
        /// ///         // Assert time domain item type is time point
        /// ///         // Write a raster: <name>.<extension>
        /// ///
        /// ///     std::vector<Index> active_set_idxs(
        /// ///         object_tracker.active_set_index().nr_arrays());
        /// ///     object_tracker.active_set_index().read(active_set_idxs.data());
        /// ///     assert(active_set_idxs.size() == 1);
        /// ///     assert(active_set_idxs.front() == 0);
        /// ///
        /// ///     std::vector<ID> active_object_ids(
        /// ///         object_tracker.active_object_id().nr_ids());
        /// ///     object_tracker.active_object_id().read(active_object_ids.data());
        /// ///     assert(active_object_ids.size() == 1);
        /// ///
        /// ///     ID const id{active_object_ids.front()};
        /// ///
        /// ///     auto const value{const_cast<Property&>(property).value()[id]};
        /// ///
        /// ///     std::cout << "property_name: " << property_name << std::endl;
        /// ///     std::cout << "id           : " << id << std::endl;
        /// ///     std::cout << "nr_arrays    : " << value.nr_arrays() << std::endl;
        /// ///     std::cout << "rank         : " << value.rank() << std::endl;
        /// ///     // TODO(KDJ) hier verder, nr_arrays == 101... klopt of niet?
        /// ///     assert(value.nr_arrays() == 1);
        /// ///     assert(value.rank() == 2);
        /// ///
        /// ///     auto const shape{value.array_shape()};
        /// ///
        /// ///     // Write a raster with the shape and values from the property value
        /// ///
        /// ///     Count const nr_bands{1};
        /// ///
        /// ///     auto gdal_dataset = create_dataset(
        /// ///         "GTiff", raster_name, shape, nr_bands, value.memory_datatype());
        /// ///
        /// ///     // FIXME
        /// ///     double const cell_size{0.000992063492063};
        /// ///     double const west{coordinates[0]};
        /// ///     double const north{coordinates[3]};
        /// ///     double geo_transform[6] = {
        /// ///         west, cell_size, 0, north, 0, -cell_size };
        /// ///
        /// ///     gdal_dataset->SetGeoTransform(geo_transform);
        /// ///
        /// ///     // TODO(KDJ)
        /// ///     // OGRSpatialReference oSRS;
        /// ///     // char *pszSRS_WKT = NULL;
        /// ///     // GDALRasterBand *poBand;
        /// ///     // GByte abyRaster[512*512];
        /// ///     // oSRS.SetUTM( 11, TRUE );
        /// ///     // oSRS.SetWellKnownGeogCS( "NAD27" );
        /// ///     // oSRS.exportToWkt( &pszSRS_WKT );
        /// ///     // poDstDS->SetProjection( pszSRS_WKT );
        /// ///     // CPLFree( pszSRS_WKT );
        /// ///
        /// ///     write_raster_band(value, 0, *gdal_dataset->GetRasterBand(1));
    }


    auto try_open_gdal_raster_dataset_for_read(std::string const& dataset_name) -> gdal::DatasetPtr
    {
        return gdal::try_open_dataset(dataset_name, GDALAccess::GA_ReadOnly);
    }


    /*!
        @brief      .
        @tparam     T In-file GDAL element type
        @return     .
        @exception  .
    */
    template<typename RasterView, typename T>
    void gdal_to_lue(
        gdal::Raster::Band& gdal_raster_band, RasterView& lue_raster_view, std::string const& layer_name)
    {
        typename RasterView::Layer lue_raster_layer = [&]()
        {
            auto const [no_data_value, has_no_data_value] = gdal_raster_band.no_data_value<T>();

            return has_no_data_value ? lue_raster_view.template add_layer<T>(layer_name, no_data_value)
                                     : lue_raster_view.template add_layer<T>(layer_name);
        }();

        auto const blocks = utility::blocks(gdal_raster_band.shape(), gdal_raster_band.block_shape());
        auto const& block_shape = blocks.block_shape();
        std::vector<T> values(gdal::nr_elements(block_shape));

        auto const [nr_blocks_y, nr_blocks_x] = blocks.shape_in_blocks();

        for (gdal::Count block_y = 0; block_y < nr_blocks_y; ++block_y)
        {
            for (gdal::Count block_x = 0; block_x < nr_blocks_x; ++block_x)
            {
                auto const [nr_valid_cells_x, nr_valid_cells_y] =
                    blocks.shape_in_valid_cells({block_x, block_y});

                hdf5::Offset offset{
                    static_cast<hdf5::Offset::value_type>(block_y * block_shape[0]),
                    static_cast<hdf5::Offset::value_type>(block_x * block_shape[1])};
                hdf5::Count count{
                    static_cast<hdf5::Count::value_type>(nr_valid_cells_y),
                    static_cast<hdf5::Count::value_type>(nr_valid_cells_x)};
                hdf5::Hyperslab hyperslab{offset, count};

                gdal_raster_band.read_block({block_y, block_x}, values.data());
                hdf5::Shape const shape = {
                    static_cast<hdf5::Shape::value_type>(nr_valid_cells_x * nr_valid_cells_y)};
                auto memory_dataspace = hdf5::create_dataspace(shape);
                lue_raster_layer.write(std::move(memory_dataspace), std::move(hyperslab), values.data());
            }
        }
    }


    template<typename RasterView>
    void gdal_to_lue(
        gdal::Raster::Band& gdal_raster_band, RasterView& lue_raster_view, std::string const& layer_name)
    {
        GDALDataType const data_type{gdal_raster_band.data_type()};

        switch (data_type)
        {
            case GDT_Byte:
            {
                gdal_to_lue<RasterView, uint8_t>(gdal_raster_band, lue_raster_view, layer_name);
                break;
            }
#if LUE_GDAL_SUPPORTS_8BIT_SIGNED_INTEGERS
            case GDT_Int8:
            {
                gdal_to_lue<RasterView, int8_t>(gdal_raster_band, lue_raster_view, layer_name);
                break;
            }
#endif
            case GDT_UInt16:
            {
                gdal_to_lue<RasterView, uint16_t>(gdal_raster_band, lue_raster_view, layer_name);
                break;
            }
            case GDT_Int16:
            {
                gdal_to_lue<RasterView, int16_t>(gdal_raster_band, lue_raster_view, layer_name);
                break;
            }
            case GDT_UInt32:
            {
                gdal_to_lue<RasterView, uint32_t>(gdal_raster_band, lue_raster_view, layer_name);
                break;
            }
            case GDT_Int32:
            {
                gdal_to_lue<RasterView, int32_t>(gdal_raster_band, lue_raster_view, layer_name);
                break;
            }
#if LUE_GDAL_SUPPORTS_64BIT_INTEGERS
            case GDT_UInt64:
            {
                gdal_to_lue<RasterView, uint64_t>(gdal_raster_band, lue_raster_view, layer_name);
                break;
            }
            case GDT_Int64:
            {
                gdal_to_lue<RasterView, int64_t>(gdal_raster_band, lue_raster_view, layer_name);
                break;
            }
#endif
            case GDT_Float32:
            {
                gdal_to_lue<RasterView, float>(gdal_raster_band, lue_raster_view, layer_name);
                break;
            }
            case GDT_Float64:
            {
                gdal_to_lue<RasterView, double>(gdal_raster_band, lue_raster_view, layer_name);
                break;
            }
            default:
            {
                throw std::runtime_error("Unsupported GDAL data type");
                break;
            }
        }
    }


    auto translate_gdal_raster_dataset_to_lue(
        std::vector<std::string> const& gdal_dataset_names,
        std::string const& lue_dataset_name,
        bool add,
        Metadata const& metadata) -> void
    {
        // TODO
        // Scenarios:
        // - Import a bunch of rasters covering the same space box. All
        //     rasters belong to the same object. For each raster a different
        //     property is made.
        // - Import a bunch of rasters covering different space boxes. All
        //     rasters belong to different objects. A single property is made.

        namespace lh5 = lue::hdf5;
        namespace lgd = lue::gdal;
        namespace ldm = lue::data_model;

        if (gdal_dataset_names.empty())
        {
            return;
        }

        // Create or open the output dataset
        auto create_dataset = [lue_dataset_name]() { return ldm::create_dataset(lue_dataset_name); };
        auto open_dataset = [lue_dataset_name, add]()
        {
            if (!add)
            {
                throw std::runtime_error(std::format("Dataset {} already exists", lue_dataset_name));
            }

            return ldm::open_dataset(lue_dataset_name);
        };

        ldm::Dataset dataset{!ldm::dataset_exists(lue_dataset_name) ? create_dataset() : open_dataset()};

        // Find information about where to write the raster to: phenomenon/property_set/property
        auto const& root_json = metadata.object();
        auto const datasets_json = json::object(root_json, "datasets");

        // Each dataset passed in is a GDAL raster dataset. Each of these can contain multiple
        // layers. Layers from rasters with the same domain and discretization can be stored in
        // the same property-set. Whether this should be done depends on the metadata passed in.
        for (std::string const& gdal_dataset_name : gdal_dataset_names)
        {
            // Find information about where to store the raster dataset in the metadata
            std::string const dataset_name{std::filesystem::path(gdal_dataset_name).stem().string()};

            auto const dataset_json = json::object(datasets_json, "name", dataset_name);
            std::string const phenomenon_name{json::string(dataset_json, "phenomenon")};
            std::string const property_set_name{json::string(dataset_json, "property_set")};
            auto const raster_json = json::object(dataset_json, "raster");

            gdal::Raster gdal_raster{gdal::open_dataset(gdal_dataset_name, GDALAccess::GA_ReadOnly)};

            auto const [nr_rows, nr_cols] = gdal_raster.shape();
            auto const [west, cell_width, row_rotation, north, col_rotation, cell_height] =
                gdal_raster.geo_transform();
            double const east = west + nr_cols * cell_width;
            double const south = north - (nr_rows * std::abs(cell_height));

            assert(east >= west);
            assert(north >= south);
            assert(nr_rows > 0);
            assert(nr_cols > 0);

            // Create / open raster view
            using RasterView = data_model::constant::RasterView<data_model::Dataset*>;
            using SpaceBox = RasterView::SpaceBox;

            lh5::Shape const grid_shape{
                static_cast<lh5::Shape::value_type>(nr_rows), static_cast<lh5::Shape::value_type>(nr_cols)};
            SpaceBox const space_box{west, south, east, north};

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

            auto const bands_json = json::object(raster_json, "bands");

            gdal::Count band_nr{1};
            for (auto it = bands_json.begin(); it != bands_json.end(); ++it, ++band_nr)
            {
                auto const band_name = json::string(*it, "name");

                gdal::Raster::Band gdal_raster_band{gdal_raster.band(band_nr)};

                gdal_to_lue<RasterView>(gdal_raster_band, raster_view, band_name);
            }
        }
    }

}  // namespace lue::utility
