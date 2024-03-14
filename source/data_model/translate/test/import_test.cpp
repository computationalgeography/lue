#define BOOST_TEST_MODULE lue translate import
#include "lue/test/print.hpp"
#include "lue/stream.hpp"
#include "lue/translate/format.hpp"
#include "lue/validate.hpp"
#include <boost/test/unit_test.hpp>
#include <numeric>


/*
    Skeleton for writing tests:
    - Define input dataset
        - Create one in-memory (e.g. GDAL driver)
        - Use an existing dataset
        - Create a dataset (e.g. Geo-JSON)
        - Use values in a JSON
        - ...
    - Import the dataset using a high-level function that is appropriate
        for the kind of input dataset
        - import from JSON
        - import from GDAL raster
        - ...
    - Verify the resulting LUE dataset contains all information
*/


BOOST_AUTO_TEST_CASE(empty_dataset)
{
    // Test importing the smallest JSON file supported

    {
        auto lue_json = R"({
"dataset": {
}
})"_json;

        auto const dataset = lue::utility::translate_json_to_lue(lue_json, "empty_dataset.lue");
        lue::data_model::assert_is_valid(dataset);

        BOOST_CHECK_EQUAL(dataset.universes().size(), 0);
        BOOST_CHECK_EQUAL(dataset.phenomena().size(), 0);
    }

    {
        auto lue_json = R"({
"dataset": {
    "phenomena": [
    ],
    "universes": [
    ]
}
})"_json;

        auto const dataset = lue::utility::translate_json_to_lue(lue_json, "empty_dataset.lue");
        lue::data_model::assert_is_valid(dataset);

        BOOST_CHECK_EQUAL(dataset.universes().size(), 0);
        BOOST_CHECK_EQUAL(dataset.phenomena().size(), 0);
    }
}


BOOST_AUTO_TEST_CASE(new_rasters)
{
    // Test importing single raster for multiple areas

    auto lue_json = R"({
    "dataset": {
        "phenomena": [
            {
                "name": "park",
                "object_id": [5, 9],
                "property_sets": [
                    {
                        "name": "area",
                        "space_domain": {
                            "datatype": "float64",
                            "rank": 2,
                            "space_box": [
                                100, 200, 400, 600,
                                200, 400, 800, 1200
                            ]
                        },
                        "properties": [
                            {
                                "name": "discretization",
                                "shape_per_object": "same_shape",
                                "value_variability": "constant",
                                "datatype": "uint64",
                                "shape": [2],
                                "value": [
                                    75, 85,
                                    95, 105
                                ]
                            },
                            {
                                "name": "dem",
                                "shape_per_object": "different_shape",
                                "value_variability": "constant",
                                "datatype": "float64",
                                "rank": 2,
                                "value": [
                                    {
                                        "id": 5,
                                        "shape": [4, 6],
                                        "value": [
                                            11.5, 12.5, 13.5, 14.5, 15.5, 16.5,
                                            21.5, 22.5, 23.5, 24.5, 25.5, 26.5,
                                            31.5, 32.5, 33.5, 34.5, 35.5, 36.5,
                                            41.5, 42.5, 43.5, 44.5, 45.5, 46.5
                                        ]
                                    },
                                    {
                                        "id": 9,
                                        "shape": [3, 5],
                                        "value": [
                                            11.5, 12.5, 13.5, 14.5, 15.5,
                                            21.5, 22.5, 23.5, 24.5, 25.5,
                                            31.5, 32.5, 33.5, 34.5, 35.5
                                        ]
                                    }
                                ],
                                "space_discretization": {
                                    "type": "regular_grid",
                                    "property": "/park/area/discretization"
                                }
                            }
                        ]
                    }
                ]
            }
        ]
    }
})"_json;

    auto dataset = lue::utility::translate_json_to_lue(lue_json, "new_raster.lue");
    lue::data_model::assert_is_valid(dataset);

    BOOST_CHECK_EQUAL(dataset.universes().size(), 0);
    BOOST_CHECK_EQUAL(dataset.phenomena().size(), 1);
    BOOST_REQUIRE(dataset.phenomena().contains("park"));

    auto& phenomenon{dataset.phenomena()["park"]};

    // Object-ID
    {
        auto& object_id{phenomenon.object_id()};
        BOOST_REQUIRE_EQUAL(object_id.nr_objects(), 2);
        std::vector<lue::data_model::Index> ids(2);
        object_id.read(lue::data_model::IndexRange{0, 2}, ids.data());
        BOOST_CHECK_EQUAL(ids[0], 5);
        BOOST_CHECK_EQUAL(ids[1], 9);
    }

    // Collection property-sets
    BOOST_CHECK(phenomenon.collection_property_sets().empty());

    // Property-sets
    BOOST_CHECK_EQUAL(phenomenon.property_sets().size(), 1);
    BOOST_REQUIRE(phenomenon.property_sets().contains("area"));

    // Property-set
    auto& property_set{phenomenon.property_sets()["area"]};

    // Object tracker
    {
        BOOST_CHECK(property_set.owns_object_tracker());

        auto const& object_tracker{property_set.object_tracker()};

        BOOST_CHECK_EQUAL(object_tracker.active_set_index().nr_indices(), 0);
        BOOST_CHECK_EQUAL(object_tracker.active_object_index().nr_indices(), 0);
        BOOST_CHECK_EQUAL(object_tracker.active_object_id().nr_ids(), 0);
    }

    // Time domain
    BOOST_CHECK(!property_set.has_time_domain());

    // Space domain
    {
        BOOST_REQUIRE(property_set.has_space_domain());

        auto const& space_domain{property_set.space_domain()};
        auto const& configuration{space_domain.configuration()};

        BOOST_REQUIRE_EQUAL(
            configuration.value<lue::data_model::Mobility>(), lue::data_model::Mobility::stationary);
        BOOST_REQUIRE_EQUAL(
            configuration.value<lue::data_model::SpaceDomainItemType>(),
            lue::data_model::SpaceDomainItemType::box);

        BOOST_CHECK(!space_domain.presence_is_discretized());

        auto value{// NOLINTNEXTLINE(cppcoreguidelines-pro-type-const-cast)
                   const_cast<lue::data_model::SpaceDomain&>(space_domain)
                       .value<lue::data_model::StationarySpaceBox>()};

        BOOST_REQUIRE_EQUAL(value.nr_boxes(), 2);
        BOOST_REQUIRE_EQUAL(value.array_shape(), lue::hdf5::Shape{4});

        BOOST_CHECK_EQUAL(value.file_datatype(), lue::hdf5::ieee_float64_le);
        BOOST_REQUIRE_EQUAL(value.memory_datatype(), lue::hdf5::native_float64);

        std::vector<double> elements(2 * 4);

        value.read(lue::data_model::IndexRange{0, 2}, elements.data());

        // Object 5
        BOOST_CHECK_EQUAL(elements[0], 100);
        BOOST_CHECK_EQUAL(elements[1], 200);
        BOOST_CHECK_EQUAL(elements[2], 400);
        BOOST_CHECK_EQUAL(elements[3], 600);

        // Object 9
        BOOST_CHECK_EQUAL(elements[4], 200);
        BOOST_CHECK_EQUAL(elements[5], 400);
        BOOST_CHECK_EQUAL(elements[6], 800);
        BOOST_CHECK_EQUAL(elements[7], 1200);
    }

    // Properties
    auto const& properties{property_set.properties()};

    BOOST_CHECK_EQUAL(properties.size(), 2);
    BOOST_REQUIRE(properties.contains("dem"));
    BOOST_REQUIRE(properties.contains("discretization"));

    // DEM
    {
        BOOST_REQUIRE_EQUAL(properties.shape_per_object("dem"), lue::data_model::ShapePerObject::different);
        BOOST_REQUIRE_EQUAL(properties.value_variability("dem"), lue::data_model::ValueVariability::constant);

        auto const& property{properties.collection<lue::data_model::different_shape::Properties>()["dem"]};

        auto const& value{property.value()};
        BOOST_REQUIRE_EQUAL(value.nr_objects(), 2);

        // Object 5
        {
            BOOST_REQUIRE(value.contains(5));

            auto const array{// NOLINTNEXTLINE(cppcoreguidelines-pro-type-const-cast)
                             const_cast<lue::data_model::different_shape::Value&>(value)[5]};

            BOOST_REQUIRE_EQUAL(array.shape(), (lue::hdf5::Shape{4, 6}));
            BOOST_CHECK_EQUAL(array.file_datatype(), lue::hdf5::ieee_float64_le);
            BOOST_REQUIRE_EQUAL(array.memory_datatype(), lue::hdf5::native_float64);

            std::vector<double> elements(24);

            array.read(elements.data());

            // Row 1
            BOOST_CHECK_EQUAL(elements[0], 11.5);
            BOOST_CHECK_EQUAL(elements[1], 12.5);
            BOOST_CHECK_EQUAL(elements[2], 13.5);
            BOOST_CHECK_EQUAL(elements[3], 14.5);
            BOOST_CHECK_EQUAL(elements[4], 15.5);
            BOOST_CHECK_EQUAL(elements[5], 16.5);

            // Row 2
            BOOST_CHECK_EQUAL(elements[6], 21.5);
            BOOST_CHECK_EQUAL(elements[7], 22.5);
            BOOST_CHECK_EQUAL(elements[8], 23.5);
            BOOST_CHECK_EQUAL(elements[9], 24.5);
            BOOST_CHECK_EQUAL(elements[10], 25.5);
            BOOST_CHECK_EQUAL(elements[11], 26.5);

            // ...

            // Row 4
            BOOST_CHECK_EQUAL(elements[18], 41.5);
            BOOST_CHECK_EQUAL(elements[19], 42.5);
            BOOST_CHECK_EQUAL(elements[20], 43.5);
            BOOST_CHECK_EQUAL(elements[21], 44.5);
            BOOST_CHECK_EQUAL(elements[22], 45.5);
            BOOST_CHECK_EQUAL(elements[23], 46.5);
        }

        // Object 9
        {
            BOOST_REQUIRE(value.contains(9));
            auto const array{// NOLINTNEXTLINE(cppcoreguidelines-pro-type-const-cast)
                             const_cast<lue::data_model::different_shape::Value&>(value)[9]};

            BOOST_REQUIRE_EQUAL(array.shape(), (lue::hdf5::Shape{3, 5}));
            BOOST_CHECK_EQUAL(array.file_datatype(), lue::hdf5::ieee_float64_le);
            BOOST_REQUIRE_EQUAL(array.memory_datatype(), lue::hdf5::native_float64);

            std::vector<double> elements(3 * 5);

            array.read(elements.data());

            // Row 1
            BOOST_CHECK_EQUAL(elements[0], 11.5);
            BOOST_CHECK_EQUAL(elements[1], 12.5);
            BOOST_CHECK_EQUAL(elements[2], 13.5);
            BOOST_CHECK_EQUAL(elements[3], 14.5);
            BOOST_CHECK_EQUAL(elements[4], 15.5);

            // Row 2
            BOOST_CHECK_EQUAL(elements[5], 21.5);
            BOOST_CHECK_EQUAL(elements[6], 22.5);
            BOOST_CHECK_EQUAL(elements[7], 23.5);
            BOOST_CHECK_EQUAL(elements[8], 24.5);
            BOOST_CHECK_EQUAL(elements[9], 25.5);

            // Row 3
            BOOST_CHECK_EQUAL(elements[10], 31.5);
            BOOST_CHECK_EQUAL(elements[11], 32.5);
            BOOST_CHECK_EQUAL(elements[12], 33.5);
            BOOST_CHECK_EQUAL(elements[13], 34.5);
            BOOST_CHECK_EQUAL(elements[14], 35.5);
        }

        // Discretization
        BOOST_CHECK(!property.time_is_discretized());
        BOOST_REQUIRE(property.space_is_discretized());
        BOOST_REQUIRE_EQUAL(
            property.space_discretization_type(), lue::data_model::SpaceDiscretization::regular_grid);

        auto discretization_property{// NOLINTNEXTLINE(cppcoreguidelines-pro-type-const-cast)
                                     const_cast<lue::data_model::different_shape::Property&>(property)
                                         .space_discretization_property()};

        BOOST_CHECK_EQUAL(discretization_property.name(), "discretization");
    }

    // Discretization
    {
        BOOST_REQUIRE_EQUAL(
            properties.shape_per_object("discretization"), lue::data_model::ShapePerObject::same);
        BOOST_REQUIRE_EQUAL(
            properties.value_variability("discretization"), lue::data_model::ValueVariability::constant);

        auto const& property{
            properties.collection<lue::data_model::same_shape::Properties>()["discretization"]};

        auto const& value{property.value()};
        BOOST_REQUIRE_EQUAL(value.nr_arrays(), 2);
        BOOST_REQUIRE_EQUAL(value.array_shape(), (lue::hdf5::Shape{2}));
        BOOST_CHECK_EQUAL(value.file_datatype(), lue::hdf5::std_uint64_le);
        BOOST_REQUIRE_EQUAL(value.memory_datatype(), lue::hdf5::native_uint64);

        std::vector<std::uint64_t> elements(2 * 2);

        value.read(elements.data());

        BOOST_CHECK_EQUAL(elements[0], 75);
        BOOST_CHECK_EQUAL(elements[1], 85);
        BOOST_CHECK_EQUAL(elements[2], 95);
        BOOST_CHECK_EQUAL(elements[3], 105);
    }
}


BOOST_AUTO_TEST_CASE(raster_round_trip_01)
{
    // Import a set of very different rasters into a LUE dataset
    // Export the same set of rasters again and verify the exported rasters contain the same
    // information as the imported ones.

    namespace ldm = lue::data_model;
    namespace lgd = lue::gdal;
    namespace lu = lue::utility;

    lgd::register_gdal_drivers();

    auto* gdal_driver_ptr = lgd::driver("GTiff");

    // - Integral element type
    // - Set some value as no-data value
    lgd::Shape const& raster_shape{6, 4};
    lue::data_model::Count const nr_bands{1};
    using Element = std::int32_t;
    GDALDataType const gdal_data_type{lgd::data_type_v<Element>};
    Element const no_data_value{5};
    std::vector<Element> elements(lgd::nr_elements(raster_shape));
    std::iota(elements.begin(), elements.end(), 1);

    // Center is at (0, 0)
    lgd::Coordinate const west{-20};
    lgd::Coordinate const north{30};
    lgd::Extent const cell_width{10};
    lgd::Extent const cell_height{10};

    lgd::GeoTransform const geo_transform{west, cell_width, 0, north, 0, -cell_height};

    // Create an input GDAL raster
    std::string const input_gdal_raster_name{"raster_round_trip_01_in.tif"};
    {
        lgd::Raster input_raster{lgd::create_dataset(
            *gdal_driver_ptr, input_gdal_raster_name, raster_shape, nr_bands, gdal_data_type)};
        lgd::Raster::Band band{input_raster.band(1)};

        // Set geo-transform
        input_raster.set_geo_transform(geo_transform);

        // Set no-data value
        band.set_no_data_value<Element>(no_data_value);

        // Set elements
        band.write(elements.data());
    }

    // Import raster into LUE data set
    std::string const lue_dataset_name{"raster_round_trip_01.lue"};
    {
        bool const add{false};

        std::stringstream metadata_stream{};
        metadata_stream <<
            R"(
                {
                    "datasets": [
                        {
                            "name": "raster_round_trip_01_in",
                            "phenomenon": "world",
                            "property_set": "field",
                            "raster": {
                                "bands": [
                                    {
                                        "name": "raster_property"
                                    }
                                ]
                            }
                        }
                    ]
                }
            )";

        lu::Metadata metadata{metadata_stream};

        if (ldm::dataset_exists(lue_dataset_name))
        {
            ldm::remove_dataset(lue_dataset_name);
        }

        lue::utility::translate_gdal_raster_dataset_to_lue(
            {input_gdal_raster_name}, lue_dataset_name, add, metadata);
    }

    // Export GDAL raster again
    std::string const output_gdal_raster_name{"raster_round_trip_01_out.tif"};
    {
        std::stringstream metadata_stream{};
        metadata_stream <<
            R"(
                {
                    "datasets": [
                        {
                            "name": "raster_round_trip_01_out",
                            "phenomenon": "world",
                            "property_set": "field",
                            "raster": {
                                "bands": [
                                    {
                                        "name": "raster_property"
                                    }
                                ]
                            }
                        }
                    ]
                }
            )";

        lu::Metadata metadata{metadata_stream};

        auto lue_dataset = ldm::open_dataset(lue_dataset_name);
        lue::utility::translate_lue_dataset_to_gdal_raster(lue_dataset, output_gdal_raster_name, metadata);
    }

    auto const differences = lgd::compare_rasters(input_gdal_raster_name, output_gdal_raster_name);
    BOOST_TEST(differences.empty(), fmt::format("{}", fmt::join(differences, ", ")));
}


BOOST_AUTO_TEST_CASE(raster_round_trip_02)
{
    // Import a temporal stack of rasters into a LUE dataset.
    // Export the stack again and verify the stack contain the same information as the imported one.


    // TODO
}
