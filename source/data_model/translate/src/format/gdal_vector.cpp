#include "lue/translate/format/gdal_vector.hpp"
#include "lue/translate/format/gdal.hpp"
#include <filesystem>


namespace lue {
namespace utility {
namespace {

bool has_extension(
    std::string const& pathname)
{
    return std::filesystem::path(pathname).has_extension();
}


std::string extension(
    std::string const& pathname)
{
    return std::filesystem::path(pathname).extension().string();
}


std::string stem(
    std::string const& pathname)
{
    return std::filesystem::path(pathname).stem().string();
}


struct OGRFeatureDeleter
{
    void operator()(::OGRFeature* feature) const
    {
        if(feature != nullptr) {
            ::OGRFeature::DestroyFeature(feature);
        }
    }
};


using OGRFeaturePtr = std::unique_ptr<::OGRFeature, OGRFeatureDeleter>;


void add_point_layer(
    ::GDALDataset& gdal_dataset,
    std::string const& layer_name,
    std::vector<double> const& space_points)
{
    auto* layer = gdal_dataset.CreateLayer(
        layer_name.c_str(), nullptr, wkbPoint, nullptr);

    if(layer == nullptr) {
        throw std::runtime_error("Cannot create layer " + layer_name);
    }

    assert(space_points.size() % 2 == 0);

    for(std::size_t i = 0; i < space_points.size(); i += 2) {
        auto feature = OGRFeaturePtr(
            OGRFeature::CreateFeature(layer->GetLayerDefn()),
            OGRFeatureDeleter{});

        double const x = space_points[i];
        double const y = space_points[i + 1];

        OGRPoint point;
        point.setX(x);
        point.setY(y);

        assert(point.IsValid());
        feature->SetGeometry(&point);

        if(layer->CreateFeature(feature.get()) != OGRERR_NONE) {
            throw std::runtime_error(
                "Cannot write feature to layer " + layer_name);
        }
    }
}


void add_box_layer(
    ::GDALDataset& gdal_dataset,
    std::string const& layer_name,
    std::vector<double> const& space_boxes)
{
    auto* layer = gdal_dataset.CreateLayer(
        layer_name.c_str(), nullptr, wkbPolygon, nullptr);

    if(layer == nullptr) {
        throw std::runtime_error("Cannot create layer " + layer_name);
    }

    assert(space_boxes.size() % 4 == 0);

    for(std::size_t i = 0; i < space_boxes.size(); i += 4) {
        auto feature = OGRFeaturePtr(
            OGRFeature::CreateFeature(layer->GetLayerDefn()),
            OGRFeatureDeleter{});

        double x_min = space_boxes[i + 0];
        double y_min = space_boxes[i + 1];
        double x_max = space_boxes[i + 2];
        double y_max = space_boxes[i + 3];

        OGRLinearRing ring;
        ring.setPoint(0, x_min, y_min);
        ring.setPoint(1, x_max, y_min);
        ring.setPoint(2, x_max, y_max);
        ring.setPoint(3, x_min, y_max);
        ring.setPoint(4, x_min, y_min);  // Close ring

        OGRPolygon polygon;
        polygon.addRing(&ring);

        assert(polygon.IsValid());
        feature->SetGeometry(&polygon);

        if(layer->CreateFeature(feature.get()) != OGRERR_NONE) {
            throw std::runtime_error(
                "Cannot write feature to layer " + layer_name);
        }
    }
}


void write_shapefile(
    std::string const& shapefile_name,
    std::string const& property_set_name,
    data_model::StationarySpaceBox const& lue_space_box)
{
    // Write a Shapefile containing space boxes

    // Create Shapefile and write information to it (possibly nothing)
    auto gdal_dataset = create_gdal_dataset("ESRI Shapefile", shapefile_name);

    assert(lue_space_box.memory_datatype() == hdf5::native_float64);

    std::vector<double> space_boxes;

    space_boxes.resize(4 * lue_space_box.nr_boxes());

    lue_space_box.read(space_boxes.data());

    std::string const& layer_name = property_set_name;
    add_box_layer(*gdal_dataset, layer_name, space_boxes);
}


void write_shapefiles(
    std::string shapefile_name,
    std::string const& property_set_name,
    data_model::ObjectTracker const& lue_object_tracker,
    data_model::TimeCell const& lue_time_cell,
    data_model::MobileSpacePoint const& lue_space_point)
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
    if(has_extension(shapefile_name)) {
        assert(extension(shapefile_name) == ".shp");
        shapefile_name = stem(shapefile_name);
    }
    assert(!has_extension(shapefile_name));

    auto gdal_dataset = create_gdal_dataset("ESRI Shapefile", shapefile_name);


    data_model::TimeCell::Count const& lue_count = lue_time_cell.count();
    assert(lue_count.nr_arrays() == 1);

    data_model::Count nr_time_cells{};
    lue_count.read(0, &nr_time_cells);

    data_model::ActiveSetIndex const& lue_active_set_idx =
        lue_object_tracker.active_set_index();
    data_model::ActiveObjectID const& lue_active_object_id =
        lue_object_tracker.active_object_id();

    assert(lue_active_set_idx.nr_indices() == nr_time_cells);

    std::vector<data_model::Index> set_idxs(lue_active_set_idx.nr_indices() + 1);
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

    data_model::Index current_set_idx = 0;
    data_model::Index next_set_idx{};


    std::string layer_name;

    for(data_model::Count c = 0; c < nr_time_cells; ++c) {

        // Figure out which space points are part of the active set at
        // time cell c
        next_set_idx = set_idxs[c + 1];
        assert(next_set_idx >= current_set_idx);

        std::size_t nr_objects = next_set_idx - current_set_idx;

        space_points.resize(2 * nr_objects);
        lue_space_point.read(
            data_model::IndexRange(
                current_set_idx, next_set_idx), space_points.data());

        layer_name = fmt::format("{}-{}", property_set_name, c + 1);
        add_point_layer(*gdal_dataset, layer_name, space_points);

        current_set_idx = next_set_idx;

    }
}

}  // Anonymous namespace


void translate_lue_dataset_to_shapefile(
    data_model::Dataset& dataset,
    std::string const& shapefile_name,
    Metadata const& metadata)
{
    // Figure out which property-sets are selected
    auto const& root_json = metadata.object();
    auto const lue_dataset_name =
        std::filesystem::path(dataset.pathname()).stem().string();

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






    data_model::Phenomenon& phenomenon = dataset.phenomena()[phenomenon_name];
    data_model::PropertySet& property_set = phenomenon.property_sets()[property_set_name];

    if(!property_set.has_space_domain()) {
        throw std::runtime_error(fmt::format(
            "Property-set {} does not have a space domain",
                property_set_name));
    }

    data_model::SpaceDomain& space_domain = property_set.space_domain();
    auto const& space_configuration = space_domain.configuration();
    auto const mobility = space_configuration.value<data_model::Mobility>();
    auto const space_domain_item_type =
        space_configuration.value<data_model::SpaceDomainItemType>();

    switch(mobility) {

        case data_model::Mobility::stationary: {

            // NOLINTNEXTLINE(hicpp-multiway-paths-covered)
            switch(space_domain_item_type) {

                case data_model::SpaceDomainItemType::box: {



                    if(!property_set.has_time_domain())
                    {
                        throw std::runtime_error(fmt::format(
                            "Translating stationary space domain with "
                            "static properties is not supported yet"));
                    }

                    // GDAL coordinates are 64-bit floats
                    auto stationary_space_box =
                        space_domain.value<data_model::StationarySpaceBox>(
                            hdf5::native_float64);

                    // TODO(KDJ)
                    bool const write_properties = false;

                    if(!write_properties) {
                        write_shapefile(
                            shapefile_name, property_set_name,
                            stationary_space_box);
                    }
                    else {

                        data_model::TimeDomain& time_domain =
                            property_set.time_domain();
                        auto const& time_configuration =
                            time_domain.configuration();
                        auto const time_domain_item_type =
                            time_configuration.value<data_model::TimeDomainItemType>();

                        // NOLINTNEXTLINE(hicpp-multiway-paths-covered)
                        switch(time_domain_item_type) {

                            // case TimeDomainItemType::cell: {

                            //     auto const time_cell =
                            //         time_domain.value<TimeCell>();

                            //     // TODO(KDJ) Add properties
                            //     write_shapefiles(
                            //         shapefile_name, property_set_name,
                            //         property_set.object_tracker(),
                            //         time_cell, stationary_space_box);

                            //     break;
                            // }
                            default: {
                                throw std::runtime_error(fmt::format(
                                    "Translating time domain with item type {} "
                                    "is not supported yet",
                                        aspect_to_string(time_domain_item_type)));
                            }
                        }
                    }




                    // TimeDomain& time_domain = property_set.time_domain();
                    // auto const& time_configuration =
                    //     time_domain.configuration();
                    // auto const time_domain_item_type =
                    //     time_configuration.value<TimeDomainItemType>();

                    // // GDAL coordinates are 64-bit floats
                    // auto mobile_space_point =
                    //     space_domain.value<MobileSpacePoint>(
                    //         hdf5::native_float64);

                    // switch(time_domain_item_type) {

                    //     case TimeDomainItemType::cell: {

                    //         auto const time_cell =
                    //             time_domain.value<TimeCell>();

                    //         // TODO(KDJ) Add properties
                    //         write_shapefiles(
                    //             shapefile_name, property_set_name,
                    //             property_set.object_tracker(),
                    //             time_cell, mobile_space_point);

                    //         break;
                    //     }
                    //     default: {
                    //         throw std::runtime_error(fmt::format(
                    //             "Translating time domain with item type {} "
                    //             "is not supported yet",
                    //                 aspect_to_string(time_domain_item_type)));
                    //     }
                    // }

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
        case data_model::Mobility::mobile: {

            // NOLINTNEXTLINE(hicpp-multiway-paths-covered)
            switch(space_domain_item_type) {

                case data_model::SpaceDomainItemType::point: {

                    assert(property_set.has_time_domain());

                    data_model::TimeDomain& time_domain = property_set.time_domain();
                    auto const& time_configuration =
                        time_domain.configuration();
                    auto const time_domain_item_type =
                        time_configuration.value<data_model::TimeDomainItemType>();

                    // GDAL coordinates are 64-bit floats
                    auto mobile_space_point =
                        space_domain.value<data_model::MobileSpacePoint>(
                            hdf5::native_float64);

                    // NOLINTNEXTLINE(hicpp-multiway-paths-covered)
                    switch(time_domain_item_type) {

                        case data_model::TimeDomainItemType::cell: {

                            auto const time_cell =
                                time_domain.value<data_model::TimeCell>();

                            // TODO(KDJ) Add properties
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
    }
}

}  // namespace utility
}  // namespace lue
