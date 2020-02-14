#include "lue/translate/format/gdal_vector.hpp"
#include "lue/translate/format/gdal.hpp"
#include <boost/filesystem.hpp>


namespace lue {
namespace utility {
namespace {

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


void add_point_layer(
    ::GDALDataset& gdal_dataset,
    std::string const& layer_name,
    std::vector<double> const& space_points)
{
    auto layer = gdal_dataset.CreateLayer(
        layer_name.c_str(), nullptr, wkbPoint, nullptr);

    if(!layer) {
        throw std::runtime_error("Cannot create layer " + layer_name);
    }

    assert(space_points.size() % 2 == 0);

    OGRPoint point;
    double x, y;

    for(std::size_t i = 0; i < space_points.size(); i += 2) {
        auto feature = OGRFeaturePtr(
            OGRFeature::CreateFeature(layer->GetLayerDefn()),
            OGRFeatureDeleter{});

        x = space_points[i];
        y = space_points[i + 1];

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
    auto layer = gdal_dataset.CreateLayer(
        layer_name.c_str(), nullptr, wkbPolygon, nullptr);

    if(!layer) {
        throw std::runtime_error("Cannot create layer " + layer_name);
    }

    assert(space_boxes.size() % 4 == 0);

    OGRLinearRing ring;
    double x_min, y_min, x_max, y_max;

    for(std::size_t i = 0; i < space_boxes.size(); i += 4) {
        auto feature = OGRFeaturePtr(
            OGRFeature::CreateFeature(layer->GetLayerDefn()),
            OGRFeatureDeleter{});

        x_min = space_boxes[i + 0];
        y_min = space_boxes[i + 1];
        x_max = space_boxes[i + 2];
        y_max = space_boxes[i + 3];

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
    StationarySpaceBox const& lue_space_box)
{
    // Write a Shapefile containing space boxes

    // Create Shapefile and write information to it (possibly nothing)
    auto gdal_dataset = create_gdal_dataset("ESRI Shapefile", shapefile_name);

    assert(lue_space_box.memory_datatype() == hdf5::native_float64);

    std::vector<double> space_boxes;

    space_boxes.resize(4 * lue_space_box.nr_boxes());

    lue_space_box.read(space_boxes.data());

    std::string const layer_name = property_set_name;
    add_box_layer(*gdal_dataset, layer_name, space_boxes);
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
    if(has_extension(shapefile_name)) {
        assert(extension(shapefile_name) == ".shp");
        shapefile_name = stem(shapefile_name);
    }
    assert(!has_extension(shapefile_name));

    auto gdal_dataset = create_gdal_dataset("ESRI Shapefile", shapefile_name);


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
        add_point_layer(*gdal_dataset, layer_name, space_points);

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

        case Mobility::stationary: {

            switch(space_domain_item_type) {

                case SpaceDomainItemType::box: {



                    if(!property_set.has_time_domain()) {

                        throw std::runtime_error(fmt::format(
                            "Translating stationary space domain with "
                            "static properties is not supported yet"));

                    }
                    else {

                        // GDAL coordinates are 64-bit floats
                        auto stationary_space_box =
                            space_domain.value<StationarySpaceBox>(
                                hdf5::native_float64);

                        // TODO
                        bool const write_properties = false;

                        if(!write_properties) {
                            write_shapefile(
                                shapefile_name, property_set_name,
                                stationary_space_box);
                        }
                        else {

                            TimeDomain& time_domain = property_set.time_domain();
                            auto const& time_configuration =
                                time_domain.configuration();
                            auto const time_domain_item_type =
                                time_configuration.value<TimeDomainItemType>();

                            switch(time_domain_item_type) {

                                // case TimeDomainItemType::cell: {

                                //     auto const time_cell =
                                //         time_domain.value<TimeCell>();

                                //     // TODO Add properties
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

                    //         // TODO Add properties
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
    }
}

}  // namespace utility
}  // namespace lue
