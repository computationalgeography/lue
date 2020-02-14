#define BOOST_TEST_MODULE lue translate import
#include <boost/test/unit_test.hpp>
#include "lue/translate/format.hpp"
#include "lue/validate.hpp"
#include "lue/test/print.hpp"


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

        auto const dataset =
            lue::utility::translate_json_to_lue(lue_json, "empty_dataset.lue");
        lue::assert_is_valid(dataset);

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

        auto const dataset =
            lue::utility::translate_json_to_lue(lue_json, "empty_dataset.lue");
        lue::assert_is_valid(dataset);

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

    auto dataset =
        lue::utility::translate_json_to_lue(lue_json, "new_raster.lue");
    lue::assert_is_valid(dataset);

    BOOST_CHECK_EQUAL(dataset.universes().size(), 0);
    BOOST_CHECK_EQUAL(dataset.phenomena().size(), 1);
    BOOST_REQUIRE(dataset.phenomena().contains("park"));

    auto& phenomenon{dataset.phenomena()["park"]};

    // Object-ID
    {
        auto& object_id{phenomenon.object_id()};
        BOOST_REQUIRE_EQUAL(object_id.nr_objects(), 2);
        std::vector<lue::Index> ids(2);
        object_id.read(lue::IndexRange{0, 2}, ids.data());
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
            configuration.value<lue::Mobility>(),
            lue::Mobility::stationary);
        BOOST_REQUIRE_EQUAL(
            configuration.value<lue::SpaceDomainItemType>(),
            lue::SpaceDomainItemType::box);

        BOOST_CHECK(!space_domain.presence_is_discretized());

        auto value{
            const_cast<lue::SpaceDomain&>(space_domain)
                .value<lue::StationarySpaceBox>()};

        BOOST_REQUIRE_EQUAL(value.nr_boxes(), 2);
        BOOST_REQUIRE_EQUAL(value.array_shape(), lue::hdf5::Shape{4});

        BOOST_CHECK_EQUAL(
            value.file_datatype(), lue::hdf5::ieee_float64_le);
        BOOST_REQUIRE_EQUAL(
            value.memory_datatype(), lue::hdf5::native_float64);

        std::vector<double> elements(2 * 4);

        value.read(lue::IndexRange{0, 2}, elements.data());

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
        BOOST_REQUIRE_EQUAL(
            properties.shape_per_object("dem"),
            lue::ShapePerObject::different);
        BOOST_REQUIRE_EQUAL(
            properties.value_variability("dem"),
            lue::ValueVariability::constant);

        auto const& property{
            properties.collection<lue::different_shape::Properties>()["dem"]};

        auto const& value{property.value()};
        BOOST_REQUIRE_EQUAL(value.nr_objects(), 2);

        // Object 5
        {
            BOOST_REQUIRE(value.contains(5));

            auto const array{const_cast<lue::different_shape::Value&>(value)[5]};

            BOOST_REQUIRE_EQUAL(array.shape(), (lue::hdf5::Shape{4, 6}));
            BOOST_CHECK_EQUAL(
                array.file_datatype(), lue::hdf5::ieee_float64_le);
            BOOST_REQUIRE_EQUAL(
                array.memory_datatype(), lue::hdf5::native_float64);

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
            BOOST_CHECK_EQUAL(elements[ 6], 21.5);
            BOOST_CHECK_EQUAL(elements[ 7], 22.5);
            BOOST_CHECK_EQUAL(elements[ 8], 23.5);
            BOOST_CHECK_EQUAL(elements[ 9], 24.5);
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
            auto const array{const_cast<lue::different_shape::Value&>(value)[9]};

            BOOST_REQUIRE_EQUAL(array.shape(), (lue::hdf5::Shape{3, 5}));
            BOOST_CHECK_EQUAL(
                array.file_datatype(), lue::hdf5::ieee_float64_le);
            BOOST_REQUIRE_EQUAL(
                array.memory_datatype(), lue::hdf5::native_float64);

            std::vector<double> elements(3 * 5);

            array.read(elements.data());

            // Row 1
            BOOST_CHECK_EQUAL(elements[ 0], 11.5);
            BOOST_CHECK_EQUAL(elements[ 1], 12.5);
            BOOST_CHECK_EQUAL(elements[ 2], 13.5);
            BOOST_CHECK_EQUAL(elements[ 3], 14.5);
            BOOST_CHECK_EQUAL(elements[ 4], 15.5);

            // Row 2
            BOOST_CHECK_EQUAL(elements[ 5], 21.5);
            BOOST_CHECK_EQUAL(elements[ 6], 22.5);
            BOOST_CHECK_EQUAL(elements[ 7], 23.5);
            BOOST_CHECK_EQUAL(elements[ 8], 24.5);
            BOOST_CHECK_EQUAL(elements[ 9], 25.5);

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
            property.space_discretization_type(),
            lue::SpaceDiscretization::regular_grid);

        auto discretization_property{
            const_cast<lue::different_shape::Property&>(property)
                .space_discretization_property()};

        BOOST_CHECK_EQUAL(discretization_property.name(), "discretization");
    }

    // Discretization
    {
        BOOST_REQUIRE_EQUAL(
            properties.shape_per_object("discretization"),
            lue::ShapePerObject::same);
        BOOST_REQUIRE_EQUAL(
            properties.value_variability("discretization"),
            lue::ValueVariability::constant);

        auto const& property{
            properties
                .collection<lue::same_shape::Properties>()["discretization"]};

        auto const& value{property.value()};
        BOOST_REQUIRE_EQUAL(value.nr_arrays(), 2);
        BOOST_REQUIRE_EQUAL(value.array_shape(), (lue::hdf5::Shape{2}));
        BOOST_CHECK_EQUAL(
            value.file_datatype(), lue::hdf5::std_uint64_le);
        BOOST_REQUIRE_EQUAL(
            value.memory_datatype(), lue::hdf5::native_uint64);

        std::vector<std::uint64_t> elements(2 * 2);

        value.read(elements.data());

        BOOST_CHECK_EQUAL(elements[0], 75);
        BOOST_CHECK_EQUAL(elements[1], 85);
        BOOST_CHECK_EQUAL(elements[2], 95);
        BOOST_CHECK_EQUAL(elements[3], 105);
    }
}
