#include "lue/translate/format/gdal.hpp"
#include "lue/translate/format/gdal_block.hpp"
#include "lue/translate/format/gdal_stacks.hpp"
#include "lue/utility/progress_indicator.hpp"
#include "lue/constant_size/time/omnipresent/space_box_domain.hpp"
#include "lue/constant_size/time/omnipresent/space_point_domain.hpp"
#include "lue/hl/raster.hpp"
#include "lue/hl/raster_stack.hpp"
#include <ogrsf_frmts.h>
#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include <cmath>
#include <iostream>
#include <map>


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


/*!
    @brief      Translate GDAL raster datasets to a LUE dataset
    @param      gdal_dataset_names Names of GDAL dataset to translate
    @param      lue_dataset_name Name of LUE dataset to create
    @exception  .

    In case a LUE dataset named @a lue_dataset_name already exists, it will
    be truncated.
*/
void translate_gdal_raster_dataset_to_lue(
    std::vector<std::string> const& gdal_dataset_names,
    std::string const& lue_dataset_name,
    Metadata const& metadata)
{
    if(gdal_dataset_names.empty()) {
        // Nothing to do.
        return;
    }

    auto lue_dataset = create_dataset(lue_dataset_name);
    auto const& root_json = metadata.object();

    for(auto gdal_dataset_name: gdal_dataset_names) {
        auto gdal_dataset =
            open_gdal_raster_dataset_for_read(gdal_dataset_name);

        gdal_dataset_name =
            boost::filesystem::path(gdal_dataset_name).stem().string();
        std::string phenomenon_name = gdal_dataset_name;
        std::string property_set_name = "area";

        auto const dataset_json = json::object(
            root_json, JSONPointer{"/datasets"}, "name", gdal_dataset_name);

        if(json::has_key(dataset_json, "phenomenon")) {
            auto const phenomenon_pointer =
                json::pointer(dataset_json, "phenomenon");
            auto const phenomenon_json =
                json::object(root_json, phenomenon_pointer);
            phenomenon_name = json::string(phenomenon_json, "name");

            if(json::has_key(phenomenon_json, "property_set")) {
                property_set_name = json::string(phenomenon_json,
                        JSON::json_pointer("/property_set/name"));
            }
        }

        GDALRaster gdal_raster{std::move(gdal_dataset)};

        auto raster = hl::create_raster(
            lue_dataset,
            phenomenon_name,
            property_set_name,
            gdal_raster.domain(),
            gdal_raster.discretization());

        // Iterate over all bands and write them to the raster.

        // Import all raster bands.
        for(std::size_t b = 1; b <= gdal_raster.nr_bands(); ++b) {
            std::string band_name = json::object(json::object(dataset_json,
                JSONPointer("/raster/bands"))[b-1], "name");

            auto gdal_raster_band = gdal_raster.band(b);
            auto const memory_datatype = gdal_raster_band.datatype();
            auto raster_band = raster.add_band(band_name, memory_datatype);

            ProgressIndicator progress_indicator(std::cout,
                "Copying blocks", gdal_raster_band.blocks().nr_blocks());
            gdal_raster_band.write(raster_band, progress_indicator);
        }

        // TODO Make sure the dataset version is in the dataset
        // TODO Write description items from the metadata
        // TODO Write metadata items
        // TODO Handle various ways of handling no-data in GDAL
    }
}


void translate_gdal_raster_stack_dataset_to_lue(
    std::vector<std::string> const& gdal_dataset_names,
    std::string const& lue_dataset_name,
    Metadata const& metadata)
{
    if(gdal_dataset_names.empty()) {
        // Nothing to do.
        return;
    }


    // For now, assume each dataset points to a stack of rasters to
    // be imported, all sharing the same temporal and spatial domain
    // and discretization
    GDALStacks gdal_stacks(gdal_dataset_names);


    // Obtain temporal domain information from the metadata
    auto const& root_json = metadata.object();

    auto lue_dataset = create_dataset(lue_dataset_name);
    std::map<std::string, std::string> phenomenon_name_by_dataset_name;
    std::map<std::string, std::string> property_set_name_by_dataset_name;
    std::map<std::string, std::string> property_name_by_dataset_name;
    std::map<std::string, std::vector<std::string>> band_name_by_dataset_name;

    // Obtain information about where to store the stack in the LUE dataset
    // from the metadata, and create the raster stacks. Data will be copied
    // later. Copying of data is expensive and we want to let the user know
    // if something's wrong in the metadata ASAP.
    for(auto const& gdal_stack: gdal_stacks) {
        std::string phenomenon_name = gdal_stack.name();
        std::string property_set_name = "area";
        std::string property_name = gdal_stack.name();

        auto const dataset_json = json::object(
            root_json, JSONPointer{"/datasets"}, "name", gdal_stack.name());
        if(json::has_key(dataset_json, "phenomenon")) {
            auto const phenomenon_pointer =
                json::pointer(dataset_json, "phenomenon");
            auto const phenomenon_json =
                json::object(root_json, phenomenon_pointer);
            phenomenon_name = json::string(phenomenon_json, "name");

            if(json::has_key(phenomenon_json, "property_set")) {
                property_set_name = json::string(phenomenon_json,
                        JSON::json_pointer("/property_set/name"));
            }
        }

        if(json::has_key(dataset_json, "property")) {
            property_name = json::string(dataset_json,
                JSON::json_pointer("/property/name"));
        }

        phenomenon_name_by_dataset_name[gdal_stack.dataset_name()] =
            phenomenon_name;
        property_set_name_by_dataset_name[gdal_stack.dataset_name()] =
            property_set_name;
        property_name_by_dataset_name[gdal_stack.dataset_name()] =
            property_name;

        auto const time_json = json::object(dataset_json, "time");
        auto const clock_json = json::object(time_json, "clock");
        auto const clock = json::clock(clock_json);
        auto const box_json = json::object(time_json, "box");
        time::DurationCount const start_time_point = box_json.at(0);
        time::DurationCount const end_time_point = box_json.at(1);

        hl::TimeSeriesDomain time_series_domain{
            clock, start_time_point, end_time_point};

        if(!hl::raster_stack_exists(
               lue_dataset, phenomenon_name, property_set_name)) {
            hl::create_raster_stack(
                lue_dataset,
                phenomenon_name,
                property_set_name,
                hl::RasterStackDomain{time_series_domain, gdal_stacks.domain()},
                gdal_stacks.discretization());
        }


        hl::RasterStack raster_stack{
            lue_dataset.phenomena()[phenomenon_name].id(),
            property_set_name};

        for(std::size_t b = 1; b <= gdal_stack.nr_bands(); ++b) {
            std::string band_name = json::object(json::object(dataset_json,
                JSONPointer("/raster/bands"))[b-1], "name");
            auto gdal_raster_stack_band = gdal_stack.band(b);
            auto const memory_datatype = gdal_raster_stack_band.datatype();
            auto raster_stack_band = raster_stack.add_band(
                band_name, memory_datatype);

            band_name_by_dataset_name[gdal_stack.dataset_name()].emplace_back(
                band_name);
        }
    }


    // For each stack, add a property to the dataset and the values themselves
    for(auto const& gdal_stack: gdal_stacks) {
        auto const phenomenon_name =
            phenomenon_name_by_dataset_name[gdal_stack.dataset_name()];
        auto const property_set_name =
            property_set_name_by_dataset_name[gdal_stack.dataset_name()];
        auto const property_name =
            property_name_by_dataset_name[gdal_stack.dataset_name()];

        hl::RasterStack raster_stack{
            lue_dataset.phenomena()[phenomenon_name].id(), property_set_name};

        // Import all raster bands.
        for(std::size_t b = 1; b <= gdal_stack.nr_bands(); ++b) {
            auto gdal_raster_stack_band = gdal_stack.band(b);
            auto const band_name =
                band_name_by_dataset_name[gdal_stack.dataset_name()][b-1];
            auto raster_stack_band = raster_stack.band(band_name);

            ProgressIndicator progress_indicator(std::cout,
                "Copying blocks", gdal_raster_stack_band.nr_blocks());
            gdal_raster_stack_band.write(raster_stack_band, progress_indicator);
        }
    }
}


// /*!
//     @brief      Translate a GDAL raster dataset to a LUE dataset
//     @param      gdal_dataset GDAL raster dataset to translate
//     @param      lue_dataset_name Name of LUE dataset to create
//     @exception  .
// 
//     In case a LUE dataset named @a lue_dataset_name already exists, it will
//     be truncated.
// */
// void translate_gdal_raster_dataset_to_lue(
//     ::GDALDataset& gdal_dataset,
//     std::string const& lue_dataset_name,
//     Metadata const& metadata)
// {
//     std::string const gdal_dataset_pathname = gdal_dataset.GetDescription();
//     auto const gdal_dataset_name =
//         boost::filesystem::path(gdal_dataset_pathname).stem().string();
//     auto const phenomenon_name = metadata.value(
//         boost::str(boost::format("/%1%/phenomenon/name") % gdal_dataset_name),
//         gdal_dataset_name);
//     auto const property_set_name = metadata.value(
//         boost::str(boost::format("/%1%/phenomenon/property_set/name")
//             % gdal_dataset_name),
//         "area");
// 
//     hl::Raster::Discretization const discretization = gdal_discretization(
//         gdal_dataset);
//     hl::Raster::Domain const domain = gdal_domain(gdal_dataset,
//         discretization);
// 
//     auto raster = hl::create_raster(lue_dataset_name, phenomenon_name,
//         property_set_name, domain, discretization);
// 
// 
//     // Iterate over all bands and write them to the raster.
//     int const nr_bands = gdal_dataset.GetRasterCount();
//     assert(nr_bands >= 0);
// 
//     // Import all raster bands.
//     for(int b = 1; b <= nr_bands; ++b) {
// 
//         GDALRasterBand* gdal_raster_band = gdal_dataset.GetRasterBand(b);
//         assert(gdal_raster_band);
//         GDALDataType const gdal_datatype =
//             gdal_raster_band->GetRasterDataType();
//         auto const memory_datatype = gdal_datatype_to_hdf5_datatype(
//             gdal_datatype);
// 
//         auto const name = metadata.value(
//             boost::str(boost::format("/%1%/raster/band/%2%/name")
//                 % gdal_dataset_name % b),
//             "band_" + std::to_string(b));
// 
//         auto raster_band = raster.add_band(name, memory_datatype);
// 
//         int block_size_x, block_size_y;
//         gdal_raster_band->GetBlockSize(&block_size_x, &block_size_y);
//         GDALBlock blocks(discretization.nr_cols(), discretization.nr_rows(),
//             block_size_x, block_size_y);
// 
//         switch(gdal_datatype) {
//             case GDT_Byte: {
//                 write_band<uint8_t>(*gdal_raster_band, blocks, raster_band);
//                 break;
//             }
//             case GDT_UInt16: {
//                 write_band<uint16_t>(*gdal_raster_band, blocks, raster_band);
//                 break;
//             }
//             case GDT_Int16: {
//                 write_band<int16_t>(*gdal_raster_band, blocks, raster_band);
//                 break;
//             }
//             case GDT_UInt32: {
//                 write_band<uint32_t>(*gdal_raster_band, blocks, raster_band);
//                 break;
//             }
//             case GDT_Int32: {
//                 write_band<int32_t>(*gdal_raster_band, blocks, raster_band);
//                 break;
//             }
//             case GDT_Float32: {
//                 write_band<float>(*gdal_raster_band, blocks, raster_band);
//                 break;
//             }
//             case GDT_Float64: {
//                 write_band<double>(*gdal_raster_band, blocks, raster_band);
//                 break;
//             }
//             default: {
//                 throw std::runtime_error("Unsupported datatype");
//                 break;
//             }
//         }
//     }
// 
//     // TODO Make sure the dataset version is in the dataset
//     // TODO Write description items from the metadata
//     // TODO Write metadata items
//     // TODO Handle various ways of handling no-data in GDAL
// }


void translate_lue_dataset_to_shapefile(
    Dataset const& dataset,
    std::string const& shapefile_name,
    Metadata const& metadata)
{
    // The correspondence of a Shapefile to a LUE dataset is as folows:
    // Single Shapefile: contains a single layer -> this function
    // Directory of Shapefiles: contains multiple Shapefiles -> not supported
    //
    // A layer can store a subset of the information that is present in
    // a LUE property set:
    // - A domain
    //     - no time
    // - Multiple properties
    //     - no discretized values
    //     - no multidimensional values

    // Figure out which property-sets to dump
    auto const& root_json = metadata.object();
    auto const lue_dataset_name =
        boost::filesystem::path(dataset.pathname()).stem().string();
    auto const dataset_json = json::object(root_json, lue_dataset_name);
    std::vector<std::tuple<std::string, JSON>> property_set_jsons;

    if(json::has_key(dataset_json, "phenomena")) {
        auto const phenomena_json = json::object(dataset_json, "phenomena");

        for(auto const& phenomenon_json: phenomena_json) {
            if(json::has_key(phenomenon_json, "property_sets")) {

            auto const property_sets_json = json::object(
                phenomenon_json, "property_sets");

                for(auto const& property_set_json: property_sets_json) {
                    property_set_jsons.emplace_back(std::make_tuple(
                        json::string(phenomenon_json, "name"),
                        property_set_json
                    ));
                }
            }
        }
    }

    if(property_set_jsons.empty()) {
        throw std::runtime_error("No property sets selected");
    }

    if(property_set_jsons.size() > 1) {
        throw std::runtime_error("More than one property set selected");
    }

    auto const property_set_json = std::get<1>(property_set_jsons.front());
    auto const property_set_name = json::string(property_set_json, "name");

    auto const phenomenon_name = std::get<0>(property_set_jsons.front());
    auto const& phenomenon = dataset.phenomena()[phenomenon_name];
    auto const& property_set = phenomenon.property_sets()[property_set_name];
    auto const& property_set_configuration = property_set.configuration();

    if(property_set_configuration.size_of_item_collection_type() !=
            SizeOfItemCollectionType::constant_size) {
        throw std::runtime_error("Size of item collection must be constant");
    }

    auto const& domain_configuration = property_set.domain().configuration();

    if(domain_configuration.domain_type() !=
            Domain::Configuration::DomainType::omnipresent) {
        throw std::runtime_error("Time domain type must be omnipresent");
    }


    // Create Shapefile
    std::string const driver_name = "ESRI Shapefile";
    auto driver = GetGDALDriverManager()->GetDriverByName(
        driver_name.c_str());

    if(driver == nullptr) {
        throw std::runtime_error(
            "Cannot obtain " + driver_name + " driver");
    }

    auto gdal_dataset = GDALDatasetPtr(
        driver->Create(shapefile_name.c_str(), 0, 0, 0, GDT_Unknown, nullptr),
        GDALDatasetDeleter{});

    if(!gdal_dataset) {
        throw std::runtime_error(
            "Cannot create Shapefile " + shapefile_name);
    }

    std::string const layer_name = property_set_name;


    // Create correct property set type
    {
        auto const property_set_ =
           constant_size::time::omnipresent::PropertySet(property_set.id());
        auto const& domain = property_set.domain();

        if(!space_domain_exists(domain)) {
            throw std::runtime_error("Space domain must exist");
        }

        auto space_domain =
           constant_size::time::omnipresent::SpaceDomain(domain);

        if(space_domain.configuration().domain_type() !=
                SpaceDomain::Configuration::DomainType::located) {
            throw std::runtime_error("Space domain type must be located");
        }

        switch(space_domain.configuration().item_type()) {
            case SpaceDomain::Configuration::ItemType::box: {
                constant_size::time::omnipresent::SpaceBoxDomain
                    space_box_domain(std::move(space_domain));

                OGRwkbGeometryType const geometry_type = wkbPolygon;

                // Create layer
                auto layer = gdal_dataset->CreateLayer(
                    layer_name.c_str(), nullptr, geometry_type, nullptr);

                if(!layer) {
                    throw std::runtime_error(
                        "Cannot create layer " + layer_name +
                        " in Shapefile " + shapefile_name);
                }


                // Create fields, but not now


                // Iterate over LUE space domain items and write OGR
                // features to layer

                auto const& boxes = space_box_domain.items();

                assert(boxes.memory_datatype() ==
                    hdf5::Datatype{H5T_NATIVE_DOUBLE});
                assert(boxes.value_shape().size() == 1);
                assert(boxes.value_shape()[0] == 4);
                std::vector<double> coordinates(
                    boxes.nr_items() * boxes.value_shape()[0]);
                boxes.read(
                    hdf5::create_dataspace(hdf5::Shape(1, coordinates.size())),
                    coordinates.data());

                for(size_t item_idx = 0; item_idx < boxes.nr_items();
                        ++item_idx) {

                    auto feature = OGRFeaturePtr(
                        OGRFeature::CreateFeature(layer->GetLayerDefn()),
                        OGRFeatureDeleter{});

                    auto x_min = coordinates[item_idx * 4 + 0];
                    auto y_min = coordinates[item_idx * 4 + 1];
                    auto x_max = coordinates[item_idx * 4 + 2];
                    auto y_max = coordinates[item_idx * 4 + 3];

                    OGRLinearRing ring;
                    ring.addPoint(x_min, y_min);
                    ring.addPoint(x_max, y_min);
                    ring.addPoint(x_max, y_max);
                    ring.addPoint(x_min, y_max);
                    ring.closeRings();

                    OGRPolygon polygon;
                    polygon.addRing(&ring);

                    feature->SetGeometry(&polygon);

                    if(layer->CreateFeature(feature.get()) != OGRERR_NONE) {
                        throw std::runtime_error(
                            "Cannot write feature to layer " + layer_name +
                            " in Shapefile " + shapefile_name);
                    }
                }

                break;
            }
            case SpaceDomain::Configuration::ItemType::point: {
                constant_size::time::omnipresent::SpacePointDomain
                    space_point_domain(std::move(space_domain));

                OGRwkbGeometryType const geometry_type = wkbPoint;

                // Create layer
                auto layer = gdal_dataset->CreateLayer(
                    layer_name.c_str(), nullptr, geometry_type, nullptr);

                if(!layer) {
                    throw std::runtime_error(
                        "Cannot create layer " + layer_name +
                        " in Shapefile " + shapefile_name);
                }


                // Create fields, but not now


                // Iterate over LUE space domain items and write OGR
                // features to layer

                auto const& domain_items = space_point_domain.items();

                assert(domain_items.memory_datatype() ==
                   hdf5::Datatype{H5T_NATIVE_DOUBLE});
                assert(domain_items.value_shape().size() == 1);
                assert(domain_items.value_shape()[0] == 2);
                std::vector<double> coordinates(
                    domain_items.nr_items() * domain_items.value_shape()[0]);
                domain_items.read(
                    hdf5::create_dataspace(hdf5::Shape(1, coordinates.size())),
                    coordinates.data());

                for(size_t item_idx = 0; item_idx < domain_items.nr_items();
                        ++item_idx) {

                    auto feature = OGRFeaturePtr(
                        OGRFeature::CreateFeature(layer->GetLayerDefn()),
                        OGRFeatureDeleter{});

                    OGRPoint point(
                        coordinates[item_idx * 2 + 0],
                        coordinates[item_idx * 2 + 1]);

                    feature->SetGeometry(&point);

                    if(layer->CreateFeature(feature.get()) != OGRERR_NONE) {
                        throw std::runtime_error(
                            "Cannot write feature to layer " + layer_name +
                            " in Shapefile " + shapefile_name);
                    }
                }

                break;
            }
            default: {
                throw std::runtime_error(
                    "Unsupported space domain item type");
            }
        }
    }
}

}  // namespace utility
}  // namespace lue
