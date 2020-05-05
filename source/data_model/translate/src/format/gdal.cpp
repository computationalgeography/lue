#include "lue/translate/format/gdal.hpp"
#include "lue/translate/format/gdal_block.hpp"
#include "lue/translate/stack_name.hpp"
// #include "lue/translate/format/gdal_stacks.hpp"
// #include "lue/utility/progress_indicator.hpp"
// #include "lue/hl/raster.hpp"
// #include "lue/hl/raster_stack.hpp"
#include "lue/data_model/hl.hpp"
#include <ogrsf_frmts.h>
#include <boost/filesystem.hpp>


namespace lue {

using namespace data_model;


namespace utility {
// namespace {
// 
// struct OGRFeatureDeleter
// {
//     void operator()(::OGRFeature* feature) const
//     {
//         if(feature) {
//             ::OGRFeature::DestroyFeature(feature);
//         }
//     }
// };
// 
// 
// using OGRFeaturePtr = std::unique_ptr<::OGRFeature, OGRFeatureDeleter>;
// 
// }  // Anonymous namespace


// /*!
//     @brief      Translate GDAL raster datasets to a LUE dataset
//     @param      gdal_dataset_names Names of GDAL dataset to translate
//     @param      lue_dataset_name Name of LUE dataset to create
//     @exception  .
// 
//     In case a LUE dataset named @a lue_dataset_name already exists, it will
//     be truncated.
// */
// void translate_gdal_raster_dataset_to_lue(
//     std::vector<std::string> const& gdal_dataset_names,
//     std::string const& lue_dataset_name,
//     Metadata const& metadata)
// {
//     if(gdal_dataset_names.empty()) {
//         // Nothing to do.
//         return;
//     }
// 
//     auto lue_dataset = create_dataset(lue_dataset_name);
//     auto const& root_json = metadata.object();
// 
//     for(auto gdal_dataset_name: gdal_dataset_names) {
//         auto gdal_dataset =
//             open_gdal_raster_dataset_for_read(gdal_dataset_name);
// 
//         gdal_dataset_name =
//             boost::filesystem::path(gdal_dataset_name).stem().string();
//         std::string phenomenon_name = gdal_dataset_name;
//         std::string property_set_name = "area";
// 
//         auto const dataset_json = json::object(
//             root_json, JSONPointer{"/datasets"}, "name", gdal_dataset_name);
// 
//         if(json::has_key(dataset_json, "phenomenon")) {
//             auto const phenomenon_pointer =
//                 json::pointer(dataset_json, "phenomenon");
//             auto const phenomenon_json =
//                 json::object(root_json, phenomenon_pointer);
//             phenomenon_name = json::string(phenomenon_json, "name");
// 
//             if(json::has_key(phenomenon_json, "property_set")) {
//                 property_set_name = json::string(phenomenon_json,
//                         JSON::json_pointer("/property_set/name"));
//             }
//         }
// 
//         GDALRaster gdal_raster{std::move(gdal_dataset)};
// 
//         auto raster = hl::create_raster(
//             lue_dataset,
//             phenomenon_name,
//             property_set_name,
//             gdal_raster.domain(),
//             gdal_raster.discretization());
// 
//         // Iterate over all bands and write them to the raster.
// 
//         // Import all raster bands.
//         for(std::size_t b = 1; b <= gdal_raster.nr_bands(); ++b) {
//             std::string band_name = json::object(json::object(dataset_json,
//                 JSONPointer("/raster/bands"))[b-1], "name");
// 
//             auto gdal_raster_band = gdal_raster.band(b);
//             auto const memory_datatype = gdal_raster_band.datatype();
//             auto raster_band = raster.add_band(band_name, memory_datatype);
// 
//             ProgressIndicator progress_indicator(std::cout,
//                 "Copying blocks", gdal_raster_band.blocks().nr_blocks());
//             gdal_raster_band.write(raster_band, progress_indicator);
//         }
// 
//         // TODO Make sure the dataset version is in the dataset
//         // TODO Write description items from the metadata
//         // TODO Write metadata items
//         // TODO Handle various ways of handling no-data in GDAL
//     }
// }
// 
// 
// void translate_gdal_raster_stack_dataset_to_lue(
//     std::vector<std::string> const& gdal_dataset_names,
//     std::string const& lue_dataset_name,
//     Metadata const& metadata)
// {
//     if(gdal_dataset_names.empty()) {
//         // Nothing to do.
//         return;
//     }
// 
// 
//     // For now, assume each dataset points to a stack of rasters to
//     // be imported, all sharing the same temporal and spatial domain
//     // and discretization
//     GDALStacks gdal_stacks(gdal_dataset_names);
// 
// 
//     // Obtain temporal domain information from the metadata
//     auto const& root_json = metadata.object();
// 
//     auto lue_dataset = create_dataset(lue_dataset_name);
//     std::map<std::string, std::string> phenomenon_name_by_dataset_name;
//     std::map<std::string, std::string> property_set_name_by_dataset_name;
//     std::map<std::string, std::string> property_name_by_dataset_name;
//     std::map<std::string, std::vector<std::string>> band_name_by_dataset_name;
// 
//     // Obtain information about where to store the stack in the LUE dataset
//     // from the metadata, and create the raster stacks. Data will be copied
//     // later. Copying of data is expensive and we want to let the user know
//     // if something's wrong in the metadata ASAP.
//     for(auto const& gdal_stack: gdal_stacks) {
//         std::string phenomenon_name = gdal_stack.name();
//         std::string property_set_name = "area";
//         std::string property_name = gdal_stack.name();
// 
//         auto const dataset_json = json::object(
//             root_json, JSONPointer{"/datasets"}, "name", gdal_stack.name());
//         if(json::has_key(dataset_json, "phenomenon")) {
//             auto const phenomenon_pointer =
//                 json::pointer(dataset_json, "phenomenon");
//             auto const phenomenon_json =
//                 json::object(root_json, phenomenon_pointer);
//             phenomenon_name = json::string(phenomenon_json, "name");
// 
//             if(json::has_key(phenomenon_json, "property_set")) {
//                 property_set_name = json::string(phenomenon_json,
//                         JSON::json_pointer("/property_set/name"));
//             }
//         }
// 
//         if(json::has_key(dataset_json, "property")) {
//             property_name = json::string(dataset_json,
//                 JSON::json_pointer("/property/name"));
//         }
// 
//         phenomenon_name_by_dataset_name[gdal_stack.dataset_name()] =
//             phenomenon_name;
//         property_set_name_by_dataset_name[gdal_stack.dataset_name()] =
//             property_set_name;
//         property_name_by_dataset_name[gdal_stack.dataset_name()] =
//             property_name;
// 
//         auto const time_json = json::object(dataset_json, "time");
//         auto const clock_json = json::object(time_json, "clock");
//         auto const clock = json::clock(clock_json);
//         auto const box_json = json::object(time_json, "box");
//         time::DurationCount const start_time_point = box_json.at(0);
//         time::DurationCount const end_time_point = box_json.at(1);
// 
//         hl::TimeSeriesDomain time_series_domain{
//             clock, start_time_point, end_time_point};
// 
//         if(!hl::raster_stack_exists(
//                lue_dataset, phenomenon_name, property_set_name)) {
//             hl::create_raster_stack(
//                 lue_dataset,
//                 phenomenon_name,
//                 property_set_name,
//                 hl::RasterStackDomain{time_series_domain, gdal_stacks.domain()},
//                 gdal_stacks.discretization());
//         }
// 
// 
//         hl::RasterStack raster_stack{
//             lue_dataset.phenomena()[phenomenon_name].id(),
//             property_set_name};
// 
//         for(std::size_t b = 1; b <= gdal_stack.nr_bands(); ++b) {
//             std::string band_name = json::object(json::object(dataset_json,
//                 JSONPointer("/raster/bands"))[b-1], "name");
//             auto gdal_raster_stack_band = gdal_stack.band(b);
//             auto const memory_datatype = gdal_raster_stack_band.datatype();
//             auto raster_stack_band = raster_stack.add_band(
//                 band_name, memory_datatype);
// 
//             band_name_by_dataset_name[gdal_stack.dataset_name()].emplace_back(
//                 band_name);
//         }
//     }
// 
// 
//     // For each stack, add a property to the dataset and the values themselves
//     for(auto const& gdal_stack: gdal_stacks) {
//         auto const phenomenon_name =
//             phenomenon_name_by_dataset_name[gdal_stack.dataset_name()];
//         auto const property_set_name =
//             property_set_name_by_dataset_name[gdal_stack.dataset_name()];
//         auto const property_name =
//             property_name_by_dataset_name[gdal_stack.dataset_name()];
// 
//         hl::RasterStack raster_stack{
//             lue_dataset.phenomena()[phenomenon_name].id(), property_set_name};
// 
//         // Import all raster bands.
//         for(std::size_t b = 1; b <= gdal_stack.nr_bands(); ++b) {
//             auto gdal_raster_stack_band = gdal_stack.band(b);
//             auto const band_name =
//                 band_name_by_dataset_name[gdal_stack.dataset_name()][b-1];
//             auto raster_stack_band = raster_stack.band(band_name);
// 
//             ProgressIndicator progress_indicator(std::cout,
//                 "Copying blocks", gdal_raster_stack_band.nr_blocks());
//             gdal_raster_stack_band.write(raster_stack_band, progress_indicator);
//         }
//     }
// }
// 
// 
// // /*!
// //     @brief      Translate a GDAL raster dataset to a LUE dataset
// //     @param      gdal_dataset GDAL raster dataset to translate
// //     @param      lue_dataset_name Name of LUE dataset to create
// //     @exception  .
// // 
// //     In case a LUE dataset named @a lue_dataset_name already exists, it will
// //     be truncated.
// // */
// // void translate_gdal_raster_dataset_to_lue(
// //     ::GDALDataset& gdal_dataset,
// //     std::string const& lue_dataset_name,
// //     Metadata const& metadata)
// // {
// //     std::string const gdal_dataset_pathname = gdal_dataset.GetDescription();
// //     auto const gdal_dataset_name =
// //         boost::filesystem::path(gdal_dataset_pathname).stem().string();
// //     auto const phenomenon_name = metadata.value(
// //         boost::str(boost::format("/%1%/phenomenon/name") % gdal_dataset_name),
// //         gdal_dataset_name);
// //     auto const property_set_name = metadata.value(
// //         boost::str(boost::format("/%1%/phenomenon/property_set/name")
// //             % gdal_dataset_name),
// //         "area");
// // 
// //     hl::Raster::Discretization const discretization = gdal_discretization(
// //         gdal_dataset);
// //     hl::Raster::Domain const domain = gdal_domain(gdal_dataset,
// //         discretization);
// // 
// //     auto raster = hl::create_raster(lue_dataset_name, phenomenon_name,
// //         property_set_name, domain, discretization);
// // 
// // 
// //     // Iterate over all bands and write them to the raster.
// //     int const nr_bands = gdal_dataset.GetRasterCount();
// //     assert(nr_bands >= 0);
// // 
// //     // Import all raster bands.
// //     for(int b = 1; b <= nr_bands; ++b) {
// // 
// //         GDALRasterBand* gdal_raster_band = gdal_dataset.GetRasterBand(b);
// //         assert(gdal_raster_band);
// //         GDALDataType const gdal_datatype =
// //             gdal_raster_band->GetRasterDataType();
// //         auto const memory_datatype = gdal_datatype_to_hdf5_datatype(
// //             gdal_datatype);
// // 
// //         auto const name = metadata.value(
// //             boost::str(boost::format("/%1%/raster/band/%2%/name")
// //                 % gdal_dataset_name % b),
// //             "band_" + std::to_string(b));
// // 
// //         auto raster_band = raster.add_band(name, memory_datatype);
// // 
// //         int block_size_x, block_size_y;
// //         gdal_raster_band->GetBlockSize(&block_size_x, &block_size_y);
// //         GDALBlock blocks(discretization.nr_cols(), discretization.nr_rows(),
// //             block_size_x, block_size_y);
// // 
// //         switch(gdal_datatype) {
// //             case GDT_Byte: {
// //                 write_band<uint8_t>(*gdal_raster_band, blocks, raster_band);
// //                 break;
// //             }
// //             case GDT_UInt16: {
// //                 write_band<uint16_t>(*gdal_raster_band, blocks, raster_band);
// //                 break;
// //             }
// //             case GDT_Int16: {
// //                 write_band<int16_t>(*gdal_raster_band, blocks, raster_band);
// //                 break;
// //             }
// //             case GDT_UInt32: {
// //                 write_band<uint32_t>(*gdal_raster_band, blocks, raster_band);
// //                 break;
// //             }
// //             case GDT_Int32: {
// //                 write_band<int32_t>(*gdal_raster_band, blocks, raster_band);
// //                 break;
// //             }
// //             case GDT_Float32: {
// //                 write_band<float>(*gdal_raster_band, blocks, raster_band);
// //                 break;
// //             }
// //             case GDT_Float64: {
// //                 write_band<double>(*gdal_raster_band, blocks, raster_band);
// //                 break;
// //             }
// //             default: {
// //                 throw std::runtime_error("Unsupported datatype");
// //                 break;
// //             }
// //         }
// //     }
// // 
// //     // TODO Make sure the dataset version is in the dataset
// //     // TODO Write description items from the metadata
// //     // TODO Write metadata items
// //     // TODO Handle various ways of handling no-data in GDAL
// // }


namespace {

// void add_point_layer(
//     ::GDALDataset& gdal_dataset,
//     std::string const& layer_name,
//     std::vector<double> const& space_points)
// {
//     auto layer = gdal_dataset.CreateLayer(
//         layer_name.c_str(), nullptr, wkbPoint, nullptr);
// 
//     if(!layer) {
//         throw std::runtime_error("Cannot create layer " + layer_name);
//     }
// 
//     assert(space_points.size() % 2 == 0);
// 
//     OGRPoint point;
//     double x, y;
// 
//     for(std::size_t i = 0; i < space_points.size(); i += 2) {
//         auto feature = OGRFeaturePtr(
//             OGRFeature::CreateFeature(layer->GetLayerDefn()),
//             OGRFeatureDeleter{});
// 
//         x = space_points[i];
//         y = space_points[i + 1];
// 
//         point.setX(x);
//         point.setY(y);
// 
//         assert(point.IsValid());
//         feature->SetGeometry(&point);
// 
//         if(layer->CreateFeature(feature.get()) != OGRERR_NONE) {
//             throw std::runtime_error(
//                 "Cannot write feature to layer " + layer_name);
//         }
//     }
// }
// 
// 
// void add_box_layer(
//     ::GDALDataset& gdal_dataset,
//     std::string const& layer_name,
//     std::vector<double> const& space_boxes)
// {
//     auto layer = gdal_dataset.CreateLayer(
//         layer_name.c_str(), nullptr, wkbPolygon, nullptr);
// 
//     if(!layer) {
//         throw std::runtime_error("Cannot create layer " + layer_name);
//     }
// 
//     assert(space_boxes.size() % 4 == 0);
// 
//     OGRLinearRing ring;
//     double x_min, y_min, x_max, y_max;
// 
//     for(std::size_t i = 0; i < space_boxes.size(); i += 4) {
//         auto feature = OGRFeaturePtr(
//             OGRFeature::CreateFeature(layer->GetLayerDefn()),
//             OGRFeatureDeleter{});
// 
//         x_min = space_boxes[i + 0];
//         y_min = space_boxes[i + 1];
//         x_max = space_boxes[i + 2];
//         y_max = space_boxes[i + 3];
// 
//         ring.setPoint(0, x_min, y_min);
//         ring.setPoint(1, x_max, y_min);
//         ring.setPoint(2, x_max, y_max);
//         ring.setPoint(3, x_min, y_max);
//         ring.setPoint(4, x_min, y_min);  // Close ring
// 
//         OGRPolygon polygon;
//         polygon.addRing(&ring);
// 
//         assert(polygon.IsValid());
//         feature->SetGeometry(&polygon);
// 
//         if(layer->CreateFeature(feature.get()) != OGRERR_NONE) {
//             throw std::runtime_error(
//                 "Cannot write feature to layer " + layer_name);
//         }
//     }
// }


// bool has_extension(
//     std::string const& pathname)
// {
//     return boost::filesystem::path(pathname).has_extension();
// }


// std::string extension(
//     std::string const& pathname)
// {
//     return boost::filesystem::path(pathname).extension().string();
// }


// std::string stem(
//     std::string const& pathname)
// {
//     return boost::filesystem::path(pathname).stem().string();
// }


GDALDataType memory_datatype_to_gdal_datatype(
    hdf5::Datatype const datatype)
{
    GDALDataType result{GDT_Unknown};

    if(datatype == hdf5::native_uint8) {
        result = GDT_Byte;
    }
    else if(datatype == hdf5::native_uint16) {
        result = GDT_UInt16;
    }
    else if(datatype == hdf5::native_int16) {
        result = GDT_Int16;
    }
    else if(datatype == hdf5::native_uint32) {
        result = GDT_UInt32;
    }
    else if(datatype == hdf5::native_int32) {
        result = GDT_Int32;
    }
    else if(datatype == hdf5::native_float32) {
        result = GDT_Float32;
    }
    else if(datatype == hdf5::native_float64) {
        result = GDT_Float64;
    }
    else {
        throw std::runtime_error("Cannot map LUE datatype to GDAL datatype");
    }

    return result;
}


auto gdal_driver_by_name(
        std::string const& name)
{
    auto driver = GetGDALDriverManager()->GetDriverByName(name.c_str());

    if(driver == nullptr) {
        throw std::runtime_error("Cannot obtain " + name + " driver");
    }

    return driver;
}


auto create_dataset(
    GDALDriver& driver,
    std::string const& dataset_name,
    hdf5::Shape const& shape,
    Count const nr_bands,
    GDALDataType const datatype)
{
    // TODO Pass in coordinate reference system
    auto dataset = GDALDatasetPtr(driver.Create(
            dataset_name.c_str(), shape[1], shape[0], nr_bands,
            datatype, nullptr),
        GDALDatasetDeleter{});

    if(!dataset) {
        throw std::runtime_error("Cannot create dataset " + dataset_name);
    }

    return dataset;
}


auto create_dataset(
    GDALDriver& driver,
    std::string const& dataset_name)
{
    return create_dataset(
        driver, dataset_name, hdf5::Shape{0, 0}, 0, GDT_Unknown);
}


auto create_dataset(
    GDALDriver& driver,
    std::string const& dataset_name,
    hdf5::Shape const& shape,
    Count const nr_bands,
    hdf5::Datatype const datatype)
{
    return create_dataset(
        driver, dataset_name, shape, nr_bands,
        memory_datatype_to_gdal_datatype(datatype));
}


GDALDatasetPtr create_dataset(
    std::string const& driver_name,
    std::string const& dataset_name,
    hdf5::Shape const& shape,
    Count const nr_bands,
    hdf5::Datatype const datatype)
{
    auto driver = gdal_driver_by_name(driver_name);
    auto dataset = create_dataset(
        *driver, dataset_name, shape, nr_bands, datatype);

    return dataset;
}


// void write_shapefile(
//     std::string const& shapefile_name,
//     std::string const& property_set_name,
//     StationarySpaceBox const& lue_space_box)
// {
//     // Write a Shapefile containing space boxes
// 
//     // Create Shapefile and write information to it (possibly nothing)
//     auto gdal_dataset = create_dataset("ESRI Shapefile", shapefile_name);
// 
//     assert(lue_space_box.memory_datatype() == hdf5::native_float64);
// 
//     std::vector<double> space_boxes;
// 
//     space_boxes.resize(4 * lue_space_box.nr_boxes());
// 
//     lue_space_box.read(space_boxes.data());
// 
//     std::string const layer_name = property_set_name;
//     add_box_layer(*gdal_dataset, layer_name, space_boxes);
// }


// void write_shapefiles(
//     std::string shapefile_name,
//     std::string const& property_set_name,
//     ObjectTracker const& lue_object_tracker,
//     TimeCell const& lue_time_cell,
//     StationarySpaceBox const& lue_space_box)
// {
//     // Write a Shapefile containing space boxes
// 
//     // Create Shapefile and write information to it (possibly nothing)
//     if(has_extension(shapefile_name)) {
//         assert(extension(shapefile_name) == ".shp");
//         shapefile_name = stem(shapefile_name);
//     }
//     assert(!has_extension(shapefile_name));
// 
//     auto gdal_dataset = create_dataset("ESRI Shapefile", shapefile_name);
// 
// }


GDALBlock natural_blocks(
    GDALRasterBand& raster_band)
{
    int block_size_x, block_size_y;

    raster_band.GetBlockSize(&block_size_x, &block_size_y);

    assert(block_size_x >= 0);
    assert(block_size_y >= 0);

    int nr_rows = raster_band.GetYSize();
    int nr_cols = raster_band.GetXSize();

    return GDALBlock{
        static_cast<std::size_t>(nr_cols),
        static_cast<std::size_t>(nr_rows),
        static_cast<std::size_t>(block_size_x),
        static_cast<std::size_t>(block_size_y)};
}


void write_raster_band_block(
    int const block_x,
    int const block_y,
    void* data,
    GDALRasterBand& raster_band)
{
    auto cpl_status = raster_band.WriteBlock(block_x, block_y, data);

    if(cpl_status != CE_None) {
        throw std::runtime_error(
            "Cannot write block to GDAL raster band");
    }
}


template<
    typename T>
void write_raster_band(
    same_shape::constant_shape::Value const& value,
    Index const time_point_idx,
    Index const time_step_idx,
    GDALRasterBand& raster_band)
{
    // It is assumed here that value contains a 2D array for multiple
    // locations in time. The 2D array to write to the raster band is
    // located at the index passed in.

    auto const blocks = natural_blocks(raster_band);

    std::vector<T> values(blocks.block_size());

    // Copy blocks for value at specific index to raster band
    for(size_t block_y = 0; block_y < blocks.nr_blocks_y(); ++block_y) {
        for(size_t block_x = 0; block_x < blocks.nr_blocks_x(); ++block_x) {

            auto const& [nr_valid_cells_x, nr_valid_cells_y] =
                blocks.nr_valid_cells(block_x, block_y);

            // hdf5::Shape const shape = {
            //     nr_valid_cells_x * nr_valid_cells_y
            // };
            // auto const memory_dataspace = hdf5::create_dataspace(shape);

            hdf5::Offset offset = {
                time_point_idx,
                time_step_idx,
                block_y * blocks.block_size_y(),
                block_x * blocks.block_size_x()};

            hdf5::Count const count = {
                1,
                1,
                nr_valid_cells_y,
                nr_valid_cells_x};

            hdf5::Hyperslab const hyperslab{offset, count};

            value.read(hyperslab, values.data());
            write_raster_band_block(
                block_x, block_y, values.data(), raster_band);
        }
    }
}


void write_raster_band(
    same_shape::constant_shape::Value const& value,
    Index const time_point_idx,
    Index const time_step_idx,
    GDALRasterBand& raster_band)
{
    auto const& memory_datatype{value.memory_datatype()};

    if(memory_datatype == hdf5::native_uint8) {
        write_raster_band<std::uint8_t>(
            value, time_point_idx, time_step_idx, raster_band);
    }
    else if(memory_datatype == hdf5::native_uint16) {
        write_raster_band<std::uint16_t>(
            value, time_point_idx, time_step_idx, raster_band);
    }
    else if(memory_datatype == hdf5::native_int16) {
        write_raster_band<std::int16_t>(
            value, time_point_idx, time_step_idx, raster_band);
    }
    else if(memory_datatype == hdf5::native_uint32) {
        write_raster_band<std::uint32_t>(
            value, time_point_idx, time_step_idx, raster_band);
    }
    else if(memory_datatype == hdf5::native_int32) {
        write_raster_band<std::int32_t>(
            value, time_point_idx, time_step_idx, raster_band);
    }
    else if(memory_datatype == hdf5::native_uint64) {
        write_raster_band<std::uint64_t>(
            value, time_point_idx, time_step_idx, raster_band);
    }
    else if(memory_datatype == hdf5::native_int64) {
        write_raster_band<std::int64_t>(
            value, time_point_idx, time_step_idx, raster_band);
    }
    else if(memory_datatype == hdf5::native_float32) {
        write_raster_band<float>(
            value, time_point_idx, time_step_idx, raster_band);
    }
    else if(memory_datatype == hdf5::native_float64) {
        write_raster_band<double>(
            value, time_point_idx, time_step_idx, raster_band);
    }
    else {
        throw std::runtime_error(
            "Cannot write value with this datatype to a GDAL raster band");
    }
}


template<
    typename T>
void write_raster_band(
    Array const& array,
    GDALRasterBand& raster_band)
{
    // It is assumed here that array only contains a 2D array

    auto const blocks = natural_blocks(raster_band);

    std::vector<T> values(blocks.block_size());

    // Copy blocks for array at specific index to raster band
    for(size_t block_y = 0; block_y < blocks.nr_blocks_y(); ++block_y) {
        for(size_t block_x = 0; block_x < blocks.nr_blocks_x(); ++block_x) {

            auto const& [nr_valid_cells_x, nr_valid_cells_y] =
                blocks.nr_valid_cells(block_x, block_y);

            // hdf5::Shape const shape = {
            //     nr_valid_cells_x * nr_valid_cells_y
            // };
            // auto const memory_dataspace = hdf5::create_dataspace(shape);

            hdf5::Offset offset = {
                block_y * blocks.block_size_y(),
                block_x * blocks.block_size_x()};

            hdf5::Count const count = {
                nr_valid_cells_y,
                nr_valid_cells_x};

            hdf5::Hyperslab const hyperslab{offset, count};

            array.read(hyperslab, values.data());
            write_raster_band_block(
                block_x, block_y, values.data(), raster_band);
        }
    }
}


void write_raster_band(
    Array const& array,
    GDALRasterBand& raster_band)
{
    auto const& memory_datatype{array.memory_datatype()};

    if(memory_datatype == hdf5::native_uint8) {
        write_raster_band<std::uint8_t>(array, raster_band);
    }
    else if(memory_datatype == hdf5::native_uint16) {
        write_raster_band<std::uint16_t>(array, raster_band);
    }
    else if(memory_datatype == hdf5::native_int16) {
        write_raster_band<std::int16_t>(array, raster_band);
    }
    else if(memory_datatype == hdf5::native_uint32) {
        write_raster_band<std::uint32_t>(array, raster_band);
    }
    else if(memory_datatype == hdf5::native_int32) {
        write_raster_band<std::int32_t>(array, raster_band);
    }
    else if(memory_datatype == hdf5::native_uint64) {
        write_raster_band<std::uint64_t>(array, raster_band);
    }
    else if(memory_datatype == hdf5::native_int64) {
        write_raster_band<std::int64_t>(array, raster_band);
    }
    else if(memory_datatype == hdf5::native_float32) {
        write_raster_band<float>(array, raster_band);
    }
    else if(memory_datatype == hdf5::native_float64) {
        write_raster_band<double>(array, raster_band);
    }
    else {
        throw std::runtime_error(
            "Cannot write value with this datatype to a GDAL raster band");
    }
}



// void write_shapefiles(
//     std::string shapefile_name,
//     std::string const& property_set_name,
//     ObjectTracker const& lue_object_tracker,
//     TimeCell const& lue_time_cell,
//     MobileSpacePoint const& lue_space_point)
// {
//     // For each time cell, write a Shapefile containing space points. Name
//     // these Shapefiles after the property-set name and time step.
//     // The result will be a directory named after the Shapefile name,
//     // without extension. In this directory the Shapefiles are stored.
// 
//     // Verify input is correct
//     if(lue_time_cell.nr_boxes() != 1) {
//         throw std::runtime_error(fmt::format(
//             "Expected 1 time box containing cells, but got {}",
//                 lue_time_cell.nr_boxes()));
//     }
// 
// 
//     // All should be fine now, create Shapefile and write information to it
//     // (possibly nothing)
//     if(has_extension(shapefile_name)) {
//         assert(extension(shapefile_name) == ".shp");
//         shapefile_name = stem(shapefile_name);
//     }
//     assert(!has_extension(shapefile_name));
// 
//     auto gdal_dataset = create_dataset("ESRI Shapefile", shapefile_name);
// 
// 
//     TimeCell::Count const& lue_count = lue_time_cell.count();
//     assert(lue_count.nr_arrays() == 1);
// 
//     Count nr_time_cells;
//     lue_count.read(0, &nr_time_cells);
// 
//     ActiveSetIndex const& lue_active_set_idx =
//         lue_object_tracker.active_set_index();
//     ActiveObjectID const& lue_active_object_id =
//         lue_object_tracker.active_object_id();
// 
//     assert(lue_active_set_idx.nr_indices() == nr_time_cells);
// 
//     std::vector<Index> set_idxs(lue_active_set_idx.nr_indices() + 1);
//     lue_active_set_idx.read(set_idxs.data());
//     set_idxs[lue_active_set_idx.nr_indices()] = lue_active_object_id.nr_ids();
// 
//     assert(lue_space_point.rank() == 1);
// 
//     if(lue_space_point.array_shape()[0] != 2) {
//         throw std::runtime_error(fmt::format(
//             "Expected space points of rank 2, but got points of rank {}",
//                 lue_space_point.array_shape()[0]));
//     }
// 
//     assert(lue_space_point.memory_datatype() == hdf5::native_float64);
// 
//     // Create buffer for points of a certain time cell
//     // To prevent reallocations later on, reserve as much memory as the
//     // maximum size of an active set. The size of each active set can
//     // be calculated by comparing adjacent active set indices.
//     std::size_t max_size_active_set = 0;
//     {
//         assert(!set_idxs.empty());
//         std::vector<std::size_t> set_sizes(set_idxs.size() - 1);
//         std::transform(
//             set_idxs.begin(), set_idxs.end() - 1,  // First input range
//             set_idxs.begin() + 1,  // Second input range
//             set_sizes.begin(),  // Destination range
//             [](
//                 auto const idx1,
//                 auto const idx2)
//             {
//                 return idx2 - idx1;
//             });
//         max_size_active_set =
//             *std::max_element(set_sizes.begin(), set_sizes.end());
//     }
// 
//     std::vector<double> space_points;
//     space_points.reserve(2 * max_size_active_set);
// 
//     Index current_set_idx = 0;
//     Index next_set_idx;
//     std::size_t nr_objects;
// 
// 
//     std::string layer_name;
// 
//     for(Count c = 0; c < nr_time_cells; ++c) {
// 
//         // Figure out which space points are part of the active set at
//         // time cell c
//         next_set_idx = set_idxs[c + 1];
//         assert(next_set_idx >= current_set_idx);
// 
//         nr_objects = next_set_idx - current_set_idx;
// 
//         space_points.resize(2 * nr_objects);
//         lue_space_point.read(
//             IndexRange(current_set_idx, next_set_idx), space_points.data());
// 
//         layer_name = fmt::format("{}-{}", property_set_name, c + 1);
//         add_point_layer(*gdal_dataset, layer_name, space_points);
// 
//         current_set_idx = next_set_idx;
// 
//     }
// }

}  // Anonymous namespace


GDALDatasetPtr create_gdal_dataset(
    std::string const& driver_name,
    std::string const& dataset_name)
{
    auto driver = gdal_driver_by_name(driver_name);
    auto dataset = create_dataset(*driver, dataset_name);

    return dataset;
}


void translate_lue_dataset_to_raster(
    Dataset& dataset,
    std::string const& raster_name,
    Metadata const& metadata)
{
    // Figure out which property-sets are selected
    auto const& root_json = metadata.object();
    auto const lue_dataset_name =
        boost::filesystem::path(dataset.pathname()).stem().string();

    // Dataset -----------------------------------------------------------------
    if(!json::has_key(root_json, lue_dataset_name)) {
        throw std::runtime_error(fmt::format(
            "No information for dataset {} present in metadata",
                lue_dataset_name));
    }

    auto const dataset_json = json::object(root_json, lue_dataset_name);

    // Phenomena ---------------------------------------------------------------
    if(!json::has_key(dataset_json, "phenomena")) {
        throw std::runtime_error(fmt::format(
            "No information about phenomena present in metadata for "
            "dataset {}",
                lue_dataset_name));
    }

    auto const phenomena_json = json::object(dataset_json, "phenomena");

    if(phenomena_json.size() != 1) {
        throw std::runtime_error(fmt::format(
            "Expected information about 1 phenomenon in metadata for "
            "dataset {}, but got {}",
                lue_dataset_name,
                phenomena_json.size()));
    }

    auto const phenomenon_json = phenomena_json.front();
    std::string const phenomenon_name = json::string(phenomenon_json, "name");

    // Property-sets -----------------------------------------------------------
    if(!json::has_key(phenomenon_json, "property_sets")) {
        throw std::runtime_error(fmt::format(
            "No information about property-sets present in metadata for "
            "phenomenon {} in dataset {}",
                phenomenon_name,
                lue_dataset_name));
    }

    auto const property_sets_json =
        json::object(phenomenon_json, "property_sets");

    if(property_sets_json.size() != 1) {
        throw std::runtime_error(fmt::format(
            "Expected information about 1 property-set in metadata for "
            "phenomenon {} in dataset {}, but got {}",
                phenomenon_name,
                lue_dataset_name,
                property_sets_json.size()));
    }

    auto const property_set_json = property_sets_json.front();
    std::string const property_set_name =
        json::string(property_set_json, "name");

    // Properties --------------------------------------------------------------
    auto const properties_json = json::object(property_set_json, "properties");

    if(properties_json.size() != 1) {
        throw std::runtime_error(fmt::format(
            "Expected information about 1 property in metadata for "
            "property-set {} in phenomenon {} in dataset {}, but got {}",
                property_set_name,
                phenomenon_name,
                lue_dataset_name,
                properties_json.size()));
    }

    auto const property_json = properties_json.front();
    std::string const property_name = json::string(property_json, "name");

    // If the constant raster view finds a raster with the property name
    // requested, export it to a single GDAL raster
    if(constant::contains_raster(
            dataset, phenomenon_name, property_set_name)) {

        using RasterView = constant::RasterView<Dataset*>;
        using RasterLayer = RasterView::Layer;

        RasterView raster_view{&dataset, phenomenon_name, property_set_name};

        if(!raster_view.contains(property_name)) {
            throw std::runtime_error(fmt::format(
                "Constant raster layer named {} is not part of property_set {}",
                property_name, property_set_name));
        }

        assert(hdf5::size_of_shape(raster_view.grid_shape()) > 0);

        Count const nr_bands{1};
        RasterLayer layer{raster_view.layer(property_name)};
        auto const& space_box{raster_view.space_box()};

        auto gdal_dataset = create_dataset(
            "GTiff", raster_name, raster_view.grid_shape(),
            nr_bands, layer.memory_datatype());

        // FIXME
        double const cell_size{0.000992063492063};
        double const west{space_box[0]};
        double const north{space_box[3]};
        double geo_transform[6] = {
            west, cell_size, 0, north, 0, -cell_size };

        gdal_dataset->SetGeoTransform(geo_transform);

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

        write_raster_band(layer, *gdal_dataset->GetRasterBand(1));
    }

    // If the variable raster view finds a raster layer with the property
    // name requested, export it to a stack of GDAL rasters
    else if(variable::contains_raster(
            dataset, phenomenon_name, property_set_name)) {

        using RasterView = variable::RasterView<Dataset*>;
        using RasterLayer = RasterView::Layer;

        RasterView raster_view{&dataset, phenomenon_name, property_set_name};

        if(!raster_view.contains(property_name)) {
            throw std::runtime_error(fmt::format(
                "Variable raster layer named {} is not part of property_set {}",
                property_name, property_set_name));
        }

        assert(hdf5::size_of_shape(raster_view.grid_shape()) > 0);

        Count const nr_bands{1};
        Index const time_point_idx{0};  // Single time box
        StackName stack_name{raster_name};
        RasterLayer layer{raster_view.layer(property_name)};
        auto const& space_box{raster_view.space_box()};

        for(Count time_step = 0; time_step < raster_view.nr_time_steps();
                ++time_step) {

            auto gdal_dataset = create_dataset(
                "GTiff", stack_name[time_step], raster_view.grid_shape(),
                nr_bands, layer.memory_datatype());

            // FIXME
            double const cell_size{0.000992063492063};
            double const west{space_box[0]};
            double const north{space_box[3]};
            double geo_transform[6] = {
                west, cell_size, 0, north, 0, -cell_size };

            gdal_dataset->SetGeoTransform(geo_transform);

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

            write_raster_band(
                layer, time_point_idx, time_step,
                *gdal_dataset->GetRasterBand(1));
        }
    }
    else {
        throw std::runtime_error(fmt::format(
            "No raster named {} found in property_set {}",
            property_name, property_set_name));
    }











///     // Find requested information in LUE dataset -------------------------------
///     Phenomenon& phenomenon = dataset.phenomena()[phenomenon_name];
///     PropertySet& property_set = phenomenon.property_sets()[property_set_name];
/// 
///     auto const& object_tracker{property_set.object_tracker()};
/// 
///     // Assert there is only a single active set
///     if(object_tracker.active_set_index().nr_indices() != 1) {
///         throw std::runtime_error("Expected a single active set");
/// 
///     }
/// 
///     // Assert the one active set contains only a single active object
///     if(object_tracker.active_object_id().nr_ids() != 1) {
///         throw std::runtime_error("Expected a single active object");
///     }
/// 
/// 
///     if(!property_set.has_space_domain()) {
///         throw std::runtime_error("Property-set does not have a space domain");
///     }
/// 
///     auto const& space_domain{property_set.space_domain()};
/// 
///     if(space_domain.configuration().value<Mobility>() != Mobility::stationary) {
///         throw std::runtime_error("Expected stationary space domain items");
///     }
/// 
///     if(space_domain.configuration().value<SpaceDomainItemType>() !=
///             SpaceDomainItemType::box) {
///         throw std::runtime_error("Expected space boxes");
///     }
/// 
///     StationarySpaceBox const space_domain_items{
///         const_cast<SpaceDomain&>(space_domain).value<StationarySpaceBox>()};
/// 
///     assert(space_domain_items.nr_boxes() == 1);
///     assert(space_domain_items.array_shape().size() == 1);
///     assert(space_domain_items.array_shape()[0] == 4);
///     assert(space_domain_items.memory_datatype() == hdf5::native_float64);
/// 
///     std::vector<double> coordinates(space_domain_items.array_shape()[0]);
///     space_domain_items.read(coordinates.data());
/// 
///     Properties& properties{property_set.properties()};
/// 
///     if(!properties.contains(property_name)) {
///         throw std::runtime_error(fmt::format(
///             "Property {} is not part of property-set {}",
///             property_name, property_set_name));
///     }
/// 
///     if(properties.shape_per_object(property_name) !=
///             ShapePerObject::different) {
///         throw std::runtime_error(
///             "Only ShapePerObject::different is currently supported");
///     }
/// 
///     if(properties.value_variability(property_name) !=
///             ValueVariability::variable) {
///         throw std::runtime_error(
///             "Only ValueVariability::variable is currently supported");
///     }
/// 
///     if(properties.shape_variability(property_name) !=
///             ShapeVariability::constant) {
///         throw std::runtime_error(
///             "Only ShapeVariability::constant is currently supported");
///     }
/// 
///     using Properties = different_shape::constant_shape::Properties;
///     using Property = different_shape::constant_shape::Property;
/// 
///     // The values of this property need to be written to a stack of rasters,
///     // using the GDAL API. Each value is a raster. Each location in time
///     // is a slice.
///     Property const& property{properties.collection<Properties>()[property_name]};
/// 
///     // For the current active set:
///     // - Iterate over each active object in the active set
/// 
///         // For the current object:
///         // Assert time domain item type is time point
///         // Write a raster: <name>.<extension>
/// 
///     std::vector<Index> active_set_idxs(
///         object_tracker.active_set_index().nr_arrays());
///     object_tracker.active_set_index().read(active_set_idxs.data());
///     assert(active_set_idxs.size() == 1);
///     assert(active_set_idxs.front() == 0);
/// 
///     std::vector<ID> active_object_ids(
///         object_tracker.active_object_id().nr_ids());
///     object_tracker.active_object_id().read(active_object_ids.data());
///     assert(active_object_ids.size() == 1);
/// 
///     ID const id{active_object_ids.front()};
/// 
///     auto const value{const_cast<Property&>(property).value()[id]};
/// 
///     std::cout << "property_name: " << property_name << std::endl;
///     std::cout << "id           : " << id << std::endl;
///     std::cout << "nr_arrays    : " << value.nr_arrays() << std::endl;
///     std::cout << "rank         : " << value.rank() << std::endl;
///     // TODO hier verder, nr_arrays == 101... klopt of niet?
///     assert(value.nr_arrays() == 1);
///     assert(value.rank() == 2);
/// 
///     auto const shape{value.array_shape()};
/// 
///     // Write a raster with the shape and values from the property value
/// 
///     Count const nr_bands{1};
/// 
///     auto gdal_dataset = create_dataset(
///         "GTiff", raster_name, shape, nr_bands, value.memory_datatype());
/// 
///     // FIXME
///     double const cell_size{0.000992063492063};
///     double const west{coordinates[0]};
///     double const north{coordinates[3]};
///     double geo_transform[6] = {
///         west, cell_size, 0, north, 0, -cell_size };
/// 
///     gdal_dataset->SetGeoTransform(geo_transform);
/// 
///     // TODO
///     // OGRSpatialReference oSRS;
///     // char *pszSRS_WKT = NULL;
///     // GDALRasterBand *poBand;
///     // GByte abyRaster[512*512];
///     // oSRS.SetUTM( 11, TRUE );
///     // oSRS.SetWellKnownGeogCS( "NAD27" );
///     // oSRS.exportToWkt( &pszSRS_WKT );
///     // poDstDS->SetProjection( pszSRS_WKT );
///     // CPLFree( pszSRS_WKT );
/// 
///     write_raster_band(value, 0, *gdal_dataset->GetRasterBand(1));
}


// void translate_lue_dataset_to_shapefile(
//     Dataset& dataset,
//     std::string const& shapefile_name,
//     Metadata const& metadata)
// {
//     // Figure out which property-sets are selected
//     auto const& root_json = metadata.object();
//     auto const lue_dataset_name =
//         boost::filesystem::path(dataset.pathname()).stem().string();
// 
//     if(!json::has_key(root_json, lue_dataset_name)) {
//         throw std::runtime_error(fmt::format(
//             "No information for dataset {} present in metadata",
//                 lue_dataset_name));
//     }
// 
//     auto const dataset_json = json::object(root_json, lue_dataset_name);
// 
//     if(!json::has_key(dataset_json, "phenomena")) {
//         throw std::runtime_error(fmt::format(
//             "No information about phenomena present in metadata for "
//             "dataset {}",
//                 lue_dataset_name));
//     }
// 
//     auto const phenomena_json = json::object(dataset_json, "phenomena");
// 
//     if(phenomena_json.size() != 1) {
//         throw std::runtime_error(fmt::format(
//             "Expected information about 1 phenomenon in metadata for "
//             "dataset {}, but got {}",
//                 lue_dataset_name,
//                 phenomena_json.size()));
//     }
// 
//     auto const phenomenon_json = phenomena_json.front();
//     std::string const phenomenon_name = json::string(phenomenon_json, "name");
// 
//     if(!json::has_key(phenomenon_json, "property_sets")) {
//         throw std::runtime_error(fmt::format(
//             "No information about property-sets present in metadata for "
//             "phenomenon {} in dataset {}",
//                 phenomenon_name,
//                 lue_dataset_name));
//     }
// 
//     auto const property_sets_json =
//         json::object(phenomenon_json, "property_sets");
// 
//     if(property_sets_json.size() != 1) {
//         throw std::runtime_error(fmt::format(
//             "Expected information about 1 property-set in metadata for "
//             "phenomenon {} in dataset {}, but got {}",
//                 phenomenon_name,
//                 lue_dataset_name,
//                 property_sets_json.size()));
//     }
// 
//     auto const property_set_json = property_sets_json.front();
//     std::string const property_set_name =
//         json::string(property_set_json, "name");
// 
// 
// 
// 
// 
// 
//     Phenomenon& phenomenon = dataset.phenomena()[phenomenon_name];
//     PropertySet& property_set = phenomenon.property_sets()[property_set_name];
// 
//     if(!property_set.has_space_domain()) {
//         throw std::runtime_error(fmt::format(
//             "Property-set {} does not have a space domain",
//                 property_set_name));
//     }
// 
//     SpaceDomain& space_domain = property_set.space_domain();
//     auto const& space_configuration = space_domain.configuration();
//     auto const mobility = space_configuration.value<Mobility>();
//     auto const space_domain_item_type =
//         space_configuration.value<SpaceDomainItemType>();
// 
//     switch(mobility) {
// 
//         case Mobility::stationary: {
// 
//             switch(space_domain_item_type) {
// 
//                 case SpaceDomainItemType::box: {
// 
// 
// 
//                     if(!property_set.has_time_domain()) {
// 
//                         throw std::runtime_error(fmt::format(
//                             "Translating stationary space domain with "
//                             "static properties is not supported yet"));
// 
//                     }
//                     else {
// 
//                         // GDAL coordinates are 64-bit floats
//                         auto stationary_space_box =
//                             space_domain.value<StationarySpaceBox>(
//                                 hdf5::native_float64);
// 
//                         // TODO
//                         bool const write_properties = false;
// 
//                         if(!write_properties) {
//                             write_shapefile(
//                                 shapefile_name, property_set_name,
//                                 stationary_space_box);
//                         }
//                         else {
// 
//                             TimeDomain& time_domain = property_set.time_domain();
//                             auto const& time_configuration =
//                                 time_domain.configuration();
//                             auto const time_domain_item_type =
//                                 time_configuration.value<TimeDomainItemType>();
// 
//                             switch(time_domain_item_type) {
// 
//                                 // case TimeDomainItemType::cell: {
// 
//                                 //     auto const time_cell =
//                                 //         time_domain.value<TimeCell>();
// 
//                                 //     // TODO Add properties
//                                 //     write_shapefiles(
//                                 //         shapefile_name, property_set_name,
//                                 //         property_set.object_tracker(),
//                                 //         time_cell, stationary_space_box);
// 
//                                 //     break;
//                                 // }
//                                 default: {
//                                     throw std::runtime_error(fmt::format(
//                                         "Translating time domain with item type {} "
//                                         "is not supported yet",
//                                             aspect_to_string(time_domain_item_type)));
//                                 }
//                             }
//                         }
//                     }
// 
// 
// 
// 
//                     // TimeDomain& time_domain = property_set.time_domain();
//                     // auto const& time_configuration =
//                     //     time_domain.configuration();
//                     // auto const time_domain_item_type =
//                     //     time_configuration.value<TimeDomainItemType>();
// 
//                     // // GDAL coordinates are 64-bit floats
//                     // auto mobile_space_point =
//                     //     space_domain.value<MobileSpacePoint>(
//                     //         hdf5::native_float64);
// 
//                     // switch(time_domain_item_type) {
// 
//                     //     case TimeDomainItemType::cell: {
// 
//                     //         auto const time_cell =
//                     //             time_domain.value<TimeCell>();
// 
//                     //         // TODO Add properties
//                     //         write_shapefiles(
//                     //             shapefile_name, property_set_name,
//                     //             property_set.object_tracker(),
//                     //             time_cell, mobile_space_point);
// 
//                     //         break;
//                     //     }
//                     //     default: {
//                     //         throw std::runtime_error(fmt::format(
//                     //             "Translating time domain with item type {} "
//                     //             "is not supported yet",
//                     //                 aspect_to_string(time_domain_item_type)));
//                     //     }
//                     // }
// 
//                     break;
// 
//                 }
//                 default: {
// 
//                     throw std::runtime_error(fmt::format(
//                         "Translating space domain with item type {} is not "
//                         "supported yet",
//                             aspect_to_string(space_domain_item_type)));
// 
//                 }
//             }
// 
//             break;
// 
//         }
//         case Mobility::mobile: {
// 
//             switch(space_domain_item_type) {
// 
//                 case SpaceDomainItemType::point: {
// 
//                     assert(property_set.has_time_domain());
// 
//                     TimeDomain& time_domain = property_set.time_domain();
//                     auto const& time_configuration =
//                         time_domain.configuration();
//                     auto const time_domain_item_type =
//                         time_configuration.value<TimeDomainItemType>();
// 
//                     // GDAL coordinates are 64-bit floats
//                     auto mobile_space_point =
//                         space_domain.value<MobileSpacePoint>(
//                             hdf5::native_float64);
// 
//                     switch(time_domain_item_type) {
// 
//                         case TimeDomainItemType::cell: {
// 
//                             auto const time_cell =
//                                 time_domain.value<TimeCell>();
// 
//                             // TODO Add properties
//                             write_shapefiles(
//                                 shapefile_name, property_set_name,
//                                 property_set.object_tracker(),
//                                 time_cell, mobile_space_point);
// 
//                             break;
//                         }
//                         default: {
//                             throw std::runtime_error(fmt::format(
//                                 "Translating time domain with item type {} "
//                                 "is not supported yet",
//                                     aspect_to_string(time_domain_item_type)));
//                         }
//                     }
// 
//                     break;
// 
//                 }
//                 default: {
// 
//                     throw std::runtime_error(fmt::format(
//                         "Translating space domain with item type {} is not "
//                         "supported yet",
//                             aspect_to_string(space_domain_item_type)));
// 
//                 }
//             }
// 
//             break;
// 
//         }
//     }
// }

}  // namespace utility
}  // namespace lue
