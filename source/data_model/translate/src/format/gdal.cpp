#include "lue/translate/format/gdal.hpp"
// #include "lue/translate/format/gdal_block.hpp"
// #include "lue/translate/format/gdal_stacks.hpp"
// #include "lue/utility/progress_indicator.hpp"
// #include "lue/hl/raster.hpp"
// #include "lue/hl/raster_stack.hpp"
#include <ogrsf_frmts.h>
#include <boost/filesystem.hpp>


namespace lue {
namespace utility {
namespace {

struct OGRFeatureDeleter
{
    void operator()(::OGRFeature* feature) const
    {
        if(feature) {
            ::OGRFeature::DestroyFeature(feature);
        }
    }
};


using OGRFeaturePtr = std::unique_ptr<::OGRFeature, OGRFeatureDeleter>;

}  // Anonymous namespace


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

void write_shapefile(
    ::GDALDataset& gdal_dataset,
    std::string const& layer_name,
    std::vector<double> const& space_points)
{
    auto layer = gdal_dataset.CreateLayer(
        layer_name.c_str(), nullptr, wkbPoint, nullptr);

    if(!layer) {
        throw std::runtime_error("Cannot create layer " + layer_name);
    }

    for(std::size_t i = 0; i < space_points.size(); i += 2) {
        auto feature = OGRFeaturePtr(
            OGRFeature::CreateFeature(layer->GetLayerDefn()),
            OGRFeatureDeleter{});

        OGRPoint point(space_points[i], space_points[i + 1]);

        feature->SetGeometry(&point);

        if(layer->CreateFeature(feature.get()) != OGRERR_NONE) {
            throw std::runtime_error(
                "Cannot write feature to layer " + layer_name);
        }
    }
}


bool has_extension(
    std::string const& pathname)
{
    return boost::filesystem::path(pathname).has_extension();
}


std::string extension(
    std::string const& pathname)
{
    return boost::filesystem::path(pathname).extension().string();
}


std::string stem(
    std::string const& pathname)
{
    return boost::filesystem::path(pathname).stem().string();
}


void write_shapefiles(
    std::string shapefile_name,
    std::string const& property_set_name,
    ObjectTracker const& lue_object_tracker,
    TimeCell const& lue_time_cell,
    MobileSpacePoint const& lue_space_point)
{
    // For each time cell, write a Shapefile containing space points. Name
    // these Shapefiles after the property-set name and time step.
    // The result will be a directory named after the Shapefile name,
    // without extension. In this directory the Shapefiles are stored.

    // Verify input is correct
    if(lue_time_cell.nr_boxes() != 1) {
        throw std::runtime_error(fmt::format(
            "Expected 1 time box containing cells, but got {}",
                lue_time_cell.nr_boxes()));
    }


    // All should be fine now, create Shapefile and write information to it
    // (possibly nothing)
    std::string const driver_name = "ESRI Shapefile";
    auto driver = GetGDALDriverManager()->GetDriverByName(driver_name.c_str());

    if(driver == nullptr) {
        throw std::runtime_error(
            "Cannot obtain " + driver_name + " driver");
    }

    if(has_extension(shapefile_name)) {
        assert(extension(shapefile_name) == ".shp");
        shapefile_name = stem(shapefile_name);
    }
    assert(!has_extension(shapefile_name));

    // TODO Pass in coordinate reference system
    auto gdal_dataset = GDALDatasetPtr(
        driver->Create(shapefile_name.c_str(), 0, 0, 0, GDT_Unknown, nullptr),
        GDALDatasetDeleter{});

    if(!gdal_dataset) {
        throw std::runtime_error(
            "Cannot create Shapefile " + shapefile_name);
    }


    TimeCell::Count const& lue_count = lue_time_cell.count();
    assert(lue_count.nr_arrays() == 1);

    Count nr_time_cells;
    lue_count.read(0, &nr_time_cells);

    ActiveSetIndex const& lue_active_set_idx =
        lue_object_tracker.active_set_index();
    ActiveObjectID const& lue_active_object_id =
        lue_object_tracker.active_object_id();

    assert(lue_active_set_idx.nr_indices() == nr_time_cells);

    std::vector<Index> set_idxs(lue_active_set_idx.nr_indices() + 1);
    lue_active_set_idx.read(set_idxs.data());
    set_idxs[lue_active_set_idx.nr_indices()] = lue_active_object_id.nr_ids();

    assert(lue_space_point.rank() == 1);

    if(lue_space_point.array_shape()[0] != 2) {
        throw std::runtime_error(fmt::format(
            "Expected space points of rank 2, but got points of rank {}",
                lue_space_point.array_shape()[0]));
    }

    assert(lue_space_point.memory_datatype() == hdf5::native_float64);

    // Create buffer for points of a certain time cell
    // To prevent reallocations later on, reserve as much memory as the
    // maximum size of an active set. The size of each active set can
    // be calculated by comparing adjacent active set indices.
    std::size_t max_size_active_set = 0;
    {
        assert(!set_idxs.empty());
        std::vector<std::size_t> set_sizes(set_idxs.size() - 1);
        std::transform(
            set_idxs.begin(), set_idxs.end() - 1,  // First input range
            set_idxs.begin() + 1,  // Second input range
            set_sizes.begin(),  // Destination range
            [](
                auto const idx1,
                auto const idx2)
            {
                return idx2 - idx1;
            });
        max_size_active_set =
            *std::max_element(set_sizes.begin(), set_sizes.end());
    }

    std::vector<double> space_points;
    space_points.reserve(2 * max_size_active_set);

    Index current_set_idx = 0;
    Index next_set_idx;
    std::size_t nr_objects;


    std::string layer_name;

    for(Count c = 0; c < nr_time_cells; ++c) {

        // Figure out which space points are part of the active set at
        // time cell c
        next_set_idx = set_idxs[c + 1];
        assert(next_set_idx >= current_set_idx);

        nr_objects = next_set_idx - current_set_idx;

        space_points.resize(2 * nr_objects);
        lue_space_point.read(
            IndexRange(current_set_idx, next_set_idx), space_points.data());

        layer_name = fmt::format("{}-{}", property_set_name, c + 1);
        write_shapefile(*gdal_dataset, layer_name, space_points);

        current_set_idx = next_set_idx;

    }
}

}  // Anonymous namespace


void translate_lue_dataset_to_shapefile(
    Dataset& dataset,
    std::string const& shapefile_name,
    Metadata const& metadata)
{
    // Figure out which property-sets are selected
    auto const& root_json = metadata.object();
    auto const lue_dataset_name =
        boost::filesystem::path(dataset.pathname()).stem().string();

    if(!json::has_key(root_json, lue_dataset_name)) {
        throw std::runtime_error(fmt::format(
            "No information for dataset {} present in metadata",
                lue_dataset_name));
    }

    auto const dataset_json = json::object(root_json, lue_dataset_name);

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

    Phenomenon& phenomenon = dataset.phenomena()[phenomenon_name];
    PropertySet& property_set = phenomenon.property_sets()[property_set_name];

    if(!property_set.has_space_domain()) {
        throw std::runtime_error(fmt::format(
            "Property-set {} does not have a space domain",
                property_set_name));
    }

    SpaceDomain& space_domain = property_set.space_domain();
    auto const& space_configuration = space_domain.configuration();
    auto const mobility = space_configuration.value<Mobility>();
    auto const space_domain_item_type =
        space_configuration.value<SpaceDomainItemType>();

    switch(mobility) {

        case Mobility::mobile: {

            switch(space_domain_item_type) {

                case SpaceDomainItemType::point: {

                    assert(property_set.has_time_domain());

                    TimeDomain& time_domain = property_set.time_domain();
                    auto const& time_configuration =
                        time_domain.configuration();
                    auto const time_domain_item_type =
                        time_configuration.value<TimeDomainItemType>();

                    // GDAL coordinates are 64-bit floats
                    auto mobile_space_point =
                        space_domain.value<MobileSpacePoint>(
                            hdf5::native_float64);

                    switch(time_domain_item_type) {

                        case TimeDomainItemType::cell: {

                            auto const time_cell =
                                time_domain.value<TimeCell>();

                            // TODO Add properties
                            write_shapefiles(
                                shapefile_name, property_set_name,
                                property_set.object_tracker(),
                                time_cell, mobile_space_point);

                            break;
                        }
                        default: {
                            throw std::runtime_error(fmt::format(
                                "Translating time domain with item type {} "
                                "is not supported yet",
                                    aspect_to_string(time_domain_item_type)));
                        }
                    }

                    break;

                }
                default: {

                    throw std::runtime_error(fmt::format(
                        "Translating space domain with item type {} is not "
                        "supported yet",
                            aspect_to_string(space_domain_item_type)));

                }
            }

            break;
        }
        default: {

            throw std::runtime_error(fmt::format(
                "Translating space domain with mobility {} is not "
                "supported yet",
                    aspect_to_string(mobility)));

        }
    }


//         switch(space_domain.configuration().type<SpaceDomainItemType>()) {
//             case SpaceDomainItemType::box: {
//                 constant_collection::time::omnipresent::SpaceBoxDomain
//                     space_box_domain(std::move(space_domain));
// 
//                 OGRwkbGeometryType const geometry_type = wkbPolygon;
// 
//                 // Create layer
//                 auto layer = gdal_dataset->CreateLayer(
//                     layer_name.c_str(), nullptr, geometry_type, nullptr);
// 
//                 if(!layer) {
//                     throw std::runtime_error(
//                         "Cannot create layer " + layer_name +
//                         " in Shapefile " + shapefile_name);
//                 }
// 
// 
//                 // Create fields, but not now
// 
// 
//                 // Iterate over LUE space domain items and write OGR
//                 // features to layer
// 
//                 auto const& boxes = space_box_domain.items();
// 
//                 assert(boxes.memory_datatype() ==
//                     hdf5::Datatype{H5T_NATIVE_DOUBLE});
//                 assert(boxes.value_shape().size() == 1);
//                 assert(boxes.value_shape()[0] == 4);
//                 std::vector<double> coordinates(
//                     boxes.nr_items() * boxes.value_shape()[0]);
//                 boxes.read(
//                     hdf5::create_dataspace(hdf5::Shape(1, coordinates.size())),
//                     coordinates.data());
// 
//                 for(size_t item_idx = 0; item_idx < boxes.nr_items();
//                         ++item_idx) {
// 
//                     auto feature = OGRFeaturePtr(
//                         OGRFeature::CreateFeature(layer->GetLayerDefn()),
//                         OGRFeatureDeleter{});
// 
//                     auto x_min = coordinates[item_idx * 4 + 0];
//                     auto y_min = coordinates[item_idx * 4 + 1];
//                     auto x_max = coordinates[item_idx * 4 + 2];
//                     auto y_max = coordinates[item_idx * 4 + 3];
// 
//                     OGRLinearRing ring;
//                     ring.addPoint(x_min, y_min);
//                     ring.addPoint(x_max, y_min);
//                     ring.addPoint(x_max, y_max);
//                     ring.addPoint(x_min, y_max);
//                     ring.closeRings();
// 
//                     OGRPolygon polygon;
//                     polygon.addRing(&ring);
// 
//                     feature->SetGeometry(&polygon);
// 
//                     if(layer->CreateFeature(feature.get()) != OGRERR_NONE) {
//                         throw std::runtime_error(
//                             "Cannot write feature to layer " + layer_name +
//                             " in Shapefile " + shapefile_name);
//                     }
//                 }
// 
//                 break;
//             }
}

}  // namespace utility
}  // namespace lue
