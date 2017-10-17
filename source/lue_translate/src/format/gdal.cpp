#include "lue/translate/format/gdal.hpp"
#include "lue/translate/format/gdal_block.hpp"
#include "lue/utility/progress_indicator.hpp"
#include "lue/constant_size/time/omnipresent/space_box_domain.hpp"
#include "lue/constant_size/time/omnipresent/space_point_domain.hpp"
#include "lue/hl/raster.hpp"
#include <ogrsf_frmts.h>
#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include <cmath>
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

    return hl::Raster::Domain(crs, west, south, east, north);
}


template<
    typename T>
void write_band(
    GDALRasterBand& gdal_raster_band,
    GDALBlock const& blocks,
    hl::Raster::Band& raster_band)
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

            if(cpl_status != CE_None) {
                throw std::runtime_error(
                    "Cannot read block from GDAL raster band");
            }

            std::tie(nr_valid_cells_x, nr_valid_cells_y) =
                blocks.nr_valid_cells(block_x, block_y);

            hdf5::Shape const shape = { nr_valid_cells_x * nr_valid_cells_y };
            auto const memory_dataspace = hdf5::create_dataspace(shape);

            hdf5::Offset start = {
                block_y * blocks.block_size_y(),
                block_x * blocks.block_size_x()
            };
            hdf5::Stride stride = { 1, 1 };
            hdf5::Count count = { nr_valid_cells_y, nr_valid_cells_x };

            raster_band.write(memory_dataspace,
                hdf5::Hyperslab(start, stride, count), values.data());

            progress_indicator.update_progress(++current_block);
        }
    }
}


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

    for(auto gdal_dataset_name: gdal_dataset_names) {
        auto gdal_dataset = try_open_gdal_raster_dataset_for_read(
                gdal_dataset_name);

        if(!gdal_dataset) {
            throw std::runtime_error(
                "Cannot open GDAL raster dataset " + gdal_dataset_name);
        }

        auto const& root_json = metadata.object();

        gdal_dataset_name =
            boost::filesystem::path(gdal_dataset_name).stem().string();
        std::string phenomenon_name = gdal_dataset_name;
        std::string property_set_name = "area";

        if(json::has_key(root_json, "datasets")) {
            auto const datasets_json = json::object(root_json, "datasets");

            auto const dataset_json_it = json::find(
                datasets_json, "name", gdal_dataset_name);

            if(dataset_json_it != datasets_json.end()) {
                auto const dataset_json = *dataset_json_it;

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
            }
        }

        hl::Raster::Discretization const discretization =
            gdal_discretization(*gdal_dataset);
        hl::Raster::Domain const domain =
            gdal_domain(*gdal_dataset, discretization);

        auto raster = hl::create_raster(lue_dataset, phenomenon_name,
            property_set_name, domain, discretization);

        // Iterate over all bands and write them to the raster.
        int const nr_bands = gdal_dataset->GetRasterCount();
        assert(nr_bands >= 0);

        // Import all raster bands.
        for(int b = 1; b <= nr_bands; ++b) {

            GDALRasterBand* gdal_raster_band = gdal_dataset->GetRasterBand(b);
            assert(gdal_raster_band);
            GDALDataType const gdal_datatype =
                gdal_raster_band->GetRasterDataType();
            auto const memory_datatype = gdal_datatype_to_hdf5_datatype(
                gdal_datatype);

            auto const name = metadata.string(
                // Object in list
                "datasets", "name", gdal_dataset_name,
                // Path in object
                JSONPointer(boost::str(boost::format(
                    "/raster/bands/%1%") % (b - 1))),
                // Key of value to obtain
                "name",
                // Default value
                "band_" + std::to_string(b));

            auto raster_band = raster.add_band(name, memory_datatype);

            int block_size_x, block_size_y;
            gdal_raster_band->GetBlockSize(&block_size_x, &block_size_y);
            assert(block_size_x >= 0);
            assert(block_size_y >= 0);
            GDALBlock blocks(
                discretization.nr_cols(), discretization.nr_rows(),
                block_size_x, block_size_y);

            switch(gdal_datatype) {
                case GDT_Byte: {
                    write_band<uint8_t>(
                        *gdal_raster_band, blocks, raster_band);
                    break;
                }
                case GDT_UInt16: {
                    write_band<uint16_t>(
                        *gdal_raster_band, blocks, raster_band);
                    break;
                }
                case GDT_Int16: {
                    write_band<int16_t>(
                        *gdal_raster_band, blocks, raster_band);
                    break;
                }
                case GDT_UInt32: {
                    write_band<uint32_t>(
                        *gdal_raster_band, blocks, raster_band);
                    break;
                }
                case GDT_Int32: {
                    write_band<int32_t>(
                        *gdal_raster_band, blocks, raster_band);
                    break;
                }
                case GDT_Float32: {
                    write_band<float>(
                        *gdal_raster_band, blocks, raster_band);
                    break;
                }
                case GDT_Float64: {
                    write_band<double>(
                        *gdal_raster_band, blocks, raster_band);
                    break;
                }
                default: {
                    throw std::runtime_error("Unsupported datatype");
                    break;
                }
            }
        }

        // TODO Make sure the dataset version is in the dataset
        // TODO Write description items from the metadata
        // TODO Write metadata items
        // TODO Handle various ways of handling no-data in GDAL
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

                assert(boxes.memory_datatype() == H5T_NATIVE_DOUBLE);
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

                assert(domain_items.memory_datatype() == H5T_NATIVE_DOUBLE);
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
