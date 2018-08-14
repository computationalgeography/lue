#define BOOST_TEST_MODULE lue raster
#include <boost/test/unit_test.hpp>
#include "lue/test.hpp"


BOOST_FIXTURE_TEST_CASE(create, lue::test::DatasetFixture)
{
    std::string const phenomenon_name = "areas";
    std::string const property_set_name = "areas";

    lue::Count const nr_areas = 10;

    // Space domain
    lue::SpaceConfiguration space_configuration{
            lue::Mobility::stationary,
            lue::SpaceDomainItemType::box
        };
    lue::hdf5::Datatype const coordinate_datatype{
        lue::hdf5::NativeDatatypeTraits<int>::type_id()};
    std::size_t const rank = 2;
    std::vector<int> boxes(nr_areas * rank * 2);
    lue::test::generate_random_values(boxes, 0, 1000);

    // IDs
    std::vector<lue::ID> ids(nr_areas);
    lue::test::generate_random_ids(ids);

    // Discretization property
    std::string const discretization_property_name = "discretization";
    lue::hdf5::Datatype const shape_datatype{
        lue::hdf5::NativeDatatypeTraits<
            lue::hdf5::Shape::value_type>::type_id()};
    std::vector<lue::hdf5::Shape::value_type> shapes(nr_areas * rank);
    lue::test::generate_random_values(shapes, 10, 20);

    // Elevation property
    std::string const elevation_property_name = "elevation";
    using ElevationValueType = double;
    lue::hdf5::Datatype const elevation_datatype{
        lue::hdf5::NativeDatatypeTraits<ElevationValueType>::type_id()};
    std::vector<std::vector<ElevationValueType>> values(nr_areas);
    for(std::size_t o = 0, s = 0; o < nr_areas; ++o, s += rank) {
        values[o].resize(shapes[s] * shapes[s + 1]);
    }
    lue::test::generate_random_values(values, 5.0, 15.0);


    // Create and write
    {
        // Phenomenon
        auto& areas = dataset().add_phenomenon(phenomenon_name);

        // Property set
        auto& area_boxes = areas.property_sets().add(
                property_set_name,
                space_configuration,
                coordinate_datatype,
                rank
            );

        // IDs
        auto& object_tracker = area_boxes.object_tracker();
        object_tracker.id().reserve(nr_areas);
        object_tracker.id().write(ids.data());

        // Time domain
        {
            // N/A
        }

        // Space domain
        {
            auto& space_domain = area_boxes.space_domain();
            auto value = space_domain.value<lue::StationarySpaceBox>();
            value.reserve(nr_areas);
            value.write(boxes.data());
        }

        // Discretization property
        auto& discretization_property = area_boxes.properties().add(
            discretization_property_name, shape_datatype,
            lue::hdf5::Shape{2});  // nr rows, nr_cols
        {
            auto& value = discretization_property.value();
            value.reserve(nr_areas);
            value.write(shapes.data());
        }

        // Elevation property
        auto& elevation_property = area_boxes.properties().add(
            elevation_property_name, elevation_datatype, rank);
        {
            auto& value = elevation_property.value();
            value.reserve(nr_areas, ids.data(), shapes.data());

            for(std::size_t o = 0; o < nr_areas; ++o) {
                value[ids[o]].write(values[o].data());
            }
        }

        // Link from elevation property to discretization property
        elevation_property.set_space_discretization(
            lue::SpaceDiscretization::regular_grid,
            discretization_property);
    }

    // Open and read
    {
        // Phenomenon
        auto& areas = dataset().phenomena()[phenomenon_name];

        //Property set
        auto& area_boxes = areas.property_sets()[property_set_name];

        // Time domain
        {
            BOOST_CHECK(!area_boxes.has_time_domain());
        }

        // Space domain
        {
            BOOST_REQUIRE(area_boxes.has_space_domain());

            auto& space_domain = area_boxes.space_domain();
            BOOST_REQUIRE_EQUAL(
                space_domain.configuration(), space_configuration);

            auto value = space_domain.value<lue::StationarySpaceBox>();

            BOOST_REQUIRE_EQUAL(value.nr_arrays(), nr_areas);
            BOOST_REQUIRE_EQUAL(value.memory_datatype(), coordinate_datatype);
            BOOST_REQUIRE_EQUAL(
                value.array_shape(), lue::hdf5::Shape{rank * 2});

            std::vector<int> boxes_read(nr_areas * rank * 2);
            value.read(boxes_read.data());

            BOOST_CHECK_EQUAL_COLLECTIONS(
                    boxes_read.begin(), boxes_read.end(),
                    boxes.begin(), boxes.end());
        }

        {
            // Elevation property
            BOOST_REQUIRE(
                area_boxes.properties().contains(elevation_property_name));
            BOOST_REQUIRE_EQUAL(
                area_boxes.properties().shape_per_object(
                    elevation_property_name),
                lue::ShapePerObject::different);
            BOOST_REQUIRE_EQUAL(
                area_boxes.properties().value_variability(
                    elevation_property_name),
                lue::ValueVariability::constant);

            auto& properties =
                area_boxes.properties().collection<lue::different_shape::Properties>();
            auto& elevation_property = properties[elevation_property_name];
            auto& value = elevation_property.value();

            BOOST_REQUIRE_EQUAL(value.nr_objects(), nr_areas);
            BOOST_REQUIRE_EQUAL(value.memory_datatype(), elevation_datatype);

            for(std::size_t o = 0; o < nr_areas; ++o) {
                auto array = value[ids[o]];

                lue::hdf5::Shape shape{shapes[o * rank], shapes[o * rank + 1]};
                BOOST_REQUIRE_EQUAL(array.shape(), shape);

                std::vector<ElevationValueType> values_read(
                    lue::hdf5::size_of_shape(array.shape()));

                array.read(values_read.data());

                BOOST_CHECK_EQUAL_COLLECTIONS(
                        values_read.begin(), values_read.end(),
                        values[o].begin(), values[o].end());
            }

            // Discretization property

            // Figure out whether property is discretized
            BOOST_REQUIRE(!elevation_property.time_is_discretized());
            BOOST_REQUIRE(elevation_property.space_is_discretized());

            // Figure out how the property is discretized
            auto discretization_type =
                elevation_property.space_discretization_type();
            BOOST_CHECK_EQUAL(
                discretization_type, lue::SpaceDiscretization::regular_grid);

            // Verify discretization parameters
            auto discretization_property =
                elevation_property.space_discretization_property();

            lue::PropertySet property_set{
                discretization_property.property_set_group()};

            BOOST_CHECK(
                property_set.properties().contains(
                    discretization_property_name));

            BOOST_REQUIRE_EQUAL(
                property_set.properties().shape_per_object(
                    discretization_property_name),
                lue::ShapePerObject::same);
            BOOST_REQUIRE_EQUAL(
                property_set.properties().value_variability(
                    discretization_property_name),
                lue::ValueVariability::constant);

            // Discretization property value
            {
                auto& properties =
                    property_set.properties().collection<lue::same_shape::Properties>();
                auto& discretization_property =
                    properties[discretization_property_name];
                auto& value = discretization_property.value();

                BOOST_REQUIRE_EQUAL(value.nr_arrays(), nr_areas);
                BOOST_REQUIRE_EQUAL(value.memory_datatype(), shape_datatype);
                BOOST_REQUIRE_EQUAL(
                    value.array_shape(), lue::hdf5::Shape{rank});

                std::vector<lue::hdf5::Shape::value_type> shapes_read(
                    nr_areas * 2);
                value.read(shapes_read.data());
                BOOST_CHECK_EQUAL_COLLECTIONS(
                    shapes_read.begin(), shapes_read.end(),
                    shapes.begin(), shapes.end());
            }
        }
    }
}
