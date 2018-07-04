#define BOOST_TEST_MODULE lue raster
#include <boost/test/unit_test.hpp>
#include "lue/test.hpp"


BOOST_FIXTURE_TEST_CASE(create, lue::test::DatasetFixture)
{
    std::string const phenomenon_name = "areas";
    std::string const property_set_name = "areas";
    std::string const elevation_property_name = "elevation";
    std::string const discretization_property_name = "discretization";

    lue::Count const nr_areas = 10;
    using ElevationValueType = double;
    lue::hdf5::Datatype const elevation_datatype{
        lue::hdf5::NativeDatatypeTraits<ElevationValueType>::type_id()};

    lue::hdf5::Datatype const shape_datatype{
        lue::hdf5::NativeDatatypeTraits<
            lue::hdf5::Shape::value_type>::type_id()};

    std::size_t const rank = 2;
    std::vector<lue::hdf5::Shape> shapes(nr_areas);
    lue::test::generate_random_shapes(shapes, rank, 10, 20);

    std::vector<lue::ID> ids(nr_areas);
    lue::test::generate_random_ids(ids);

    std::vector<std::vector<ElevationValueType>> values(nr_areas);
    for(std::size_t o = 0; o < nr_areas; ++o) {
        values[o].resize(lue::hdf5::size_of_shape(shapes[o]));
    }
    lue::test::generate_random_values(values, 5.0, 15.0);

    // Create and write
    {
        auto& areas = dataset().add_phenomenon(phenomenon_name);
        auto& area_boxes = areas.property_sets().add(
                property_set_name,
                lue::SpaceConfiguration{
                    lue::Mobility::stationary,
                    lue::SpaceDomainItemType::box
                }
            );
        auto& object_tracker = area_boxes.object_tracker();

        object_tracker.id().reserve(nr_areas);
        object_tracker.id().write(ids.data());

        auto& elevation_property = area_boxes.properties().add(
            elevation_property_name, elevation_datatype, rank);

        // Elevation property value
        {
            auto& value = elevation_property.value();

            value.reserve(nr_areas, ids.data(), shapes.data());

            for(std::size_t o = 0; o < nr_areas; ++o) {
                value[ids[o]].write(values[o].data());
            }
        }

        // Discretization property
        {
            auto& discretization_property = area_boxes.properties().add(
                discretization_property_name, shape_datatype,
                lue::hdf5::Shape{2});
            auto& value = discretization_property.value();

            value.reserve(nr_areas);
            value.write(shapes.data());

            elevation_property.set_space_discretisation(
                lue::SpaceDiscretization::cartesian_grid,
                discretization_property);
        }
    }

    // Open and read
    {
        auto& areas = dataset().phenomena()[phenomenon_name];
        auto& area_boxes = areas.property_sets()[property_set_name];

        BOOST_REQUIRE(area_boxes.properties().contains(elevation_property_name));

        auto shape_per_object =
            area_boxes.properties().shape_per_object(elevation_property_name);
        auto value_variability =
            area_boxes.properties().value_variability(elevation_property_name);

        BOOST_REQUIRE(shape_per_object == lue::ShapePerObject::different);
        BOOST_REQUIRE(value_variability == lue::ValueVariability::constant);

        auto& properties =
            area_boxes.properties().collection<lue::different_shape::Properties>();

        auto& elevation_property = properties[elevation_property_name];

        // Elevation property value
        {
            auto& value = elevation_property.value();

            BOOST_REQUIRE_EQUAL(value.nr_objects(), nr_areas);
            BOOST_REQUIRE(value.memory_datatype() == elevation_datatype);

            std::vector<lue::hdf5::Shape> shapes_read(nr_areas);

            for(std::size_t o = 0; o < nr_areas; ++o) {
                auto array = value[ids[o]];

                BOOST_REQUIRE(array.shape() == shapes[o]);

                std::vector<ElevationValueType> values_read(
                    lue::hdf5::size_of_shape(array.shape()));

                array.read(values_read.data());

                BOOST_CHECK_EQUAL_COLLECTIONS(
                        values_read.begin(), values_read.end(),
                        values[o].begin(), values[o].end());
            }
        }

        // Discretization property
        {
            // Figure out whether property is discretized
            BOOST_REQUIRE(elevation_property.space_is_discretized());

            // Figure out how the property is discretized
            auto discretization_type =
                elevation_property.space_discretization_type();
            BOOST_CHECK(
                discretization_type ==
                lue::SpaceDiscretization::cartesian_grid);

            // Verify discretization parameters
            auto const discretization_property =
                elevation_property.space_discretization_property();

            // auto const& property_set_id =
            //     discretization_property.property_set_id();

            // We need to instantiate the correct property class. How
            // to obtain information about the property when all we have
            // is a property
            // - through property set (move up the hierarchy)?
            // - through property itself?
            // In any case, we need to be able to get at the property
            // set containing a property.
        }
    }
}
