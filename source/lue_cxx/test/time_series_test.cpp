#define BOOST_TEST_MODULE lue time_series
#include <boost/test/unit_test.hpp>
#include "lue/test.hpp"


BOOST_FIXTURE_TEST_CASE(create, lue::test::DatasetFixture)
{
    std::string const phenomenon_name = "areas";
    std::string const property_set_name = "outlets";
    std::string const collection_property_set_name = "outlets_collection";

    lue::Count const nr_areas = 10;
    lue::Count const nr_time_boxes = 3;

    // IDs
    // TODO Collections of IDs of active objects
    // std::vector<lue::ID> ids(nr_areas);
    // lue::test::generate_random_ids(ids);

    // Time domain
    lue::TimeConfiguration time_configuration{
            lue::TimeDomainItemType::box
        };
    lue::Clock const clock{lue::time::Unit::day, 1};
    using TimeCoordinateValueType = lue::time::TickPeriodCount;
    lue::hdf5::Datatype const time_coordinate_datatype{
        lue::hdf5::NativeDatatypeTraits<TimeCoordinateValueType>::type_id()};
    std::vector<TimeCoordinateValueType> time_boxes(nr_time_boxes * 2);
    lue::test::generate_random_values(time_boxes, 0, 1000);

    // Space domain
    lue::SpaceConfiguration space_configuration{
            lue::Mobility::stationary,
            lue::SpaceDomainItemType::point
        };
    using SpaceCoordinateValueType = double;
    lue::hdf5::Datatype const space_coordinate_datatype{
        lue::hdf5::NativeDatatypeTraits<SpaceCoordinateValueType>::type_id()};
    std::size_t const rank = 2;
    std::vector<SpaceCoordinateValueType> space_points(nr_areas * rank);
    lue::test::generate_random_values(space_points, 0, 1000);

    // Discharge property
    std::string const discharge_property_name = "discharge";
    using DischargeValueType = float;
    lue::hdf5::Datatype const discharge_datatype{
        lue::hdf5::NativeDatatypeTraits<DischargeValueType>::type_id()};
    std::vector<DischargeValueType> values(nr_areas * nr_time_boxes);
    lue::test::generate_random_values(values, 0.0, 1500.0);

    // Discretization property
    std::string const discretization_property_name = "discretization";
    lue::hdf5::Datatype const shape_datatype{
        lue::hdf5::NativeDatatypeTraits<
            lue::hdf5::Shape::value_type>::type_id()};
    std::vector<lue::hdf5::Shape::value_type> shapes(nr_time_boxes);
    lue::test::generate_random_values(shapes, 10, 20);

    // Create and write
    {
        // Phenomenon
        auto& areas = dataset().add_phenomenon(phenomenon_name);

        // Discharge property --------------------------------------------------
        {
            // Property set
            auto& outlet_points = areas.property_sets().add(
                    property_set_name,
                    time_configuration,
                    clock,
                    space_configuration,
                    space_coordinate_datatype,
                    rank
                );

            // IDs
            // TODO track id through time!!!
            {
                /* auto& object_tracker = */ outlet_points.object_tracker();
                // ...

                // object_tracker.id().reserve(nr_areas);
                // object_tracker.id().write(ids.data());
            }

            // Time domain
            {
                auto& time_domain = outlet_points.time_domain();
                auto value = time_domain.value<lue::TimeBox>();
                value.reserve(nr_time_boxes);
                value.write(time_boxes.data());
            }

            // Space domain
            {
                auto& space_domain = outlet_points.space_domain();
                auto value = space_domain.value<lue::StationarySpacePoint>();
                value.reserve(nr_areas);
                value.write(space_points.data());
            }

            // Property
            {
                using Property = lue::same_shape::constant_shape::Property;
                auto& properties = outlet_points.properties();
                auto& discharge_property = properties.add<Property>(
                    discharge_property_name, discharge_datatype);
                auto& value = discharge_property.value();
                value.reserve(nr_areas * nr_time_boxes);
                value.write(values.data());
            }
        }

        // Discretization property ---------------------------------------------
        {
            // Property set
            auto& collection = areas.collection_property_sets().add(
                    collection_property_set_name,
                    // Time domain from discharge property
                    areas.property_sets()[property_set_name].time_domain()
                );

            // IDs
            // TODO
            {
                /* auto& object_tracker = */ collection.object_tracker();
                // ...
            }

            // Property
            {
                using Property = lue::same_shape::constant_shape::Property;
                auto& properties = collection.properties();
                auto& discretization_property = properties.add<Property>(
                    discretization_property_name, shape_datatype,
                    lue::hdf5::Shape{1});  // nr time steps
                auto& value = discretization_property.value();
                value.reserve(nr_time_boxes);
                value.write(shapes.data());
            }
        }

        // Link from discharge property to discretization property
        {
            auto& discharge_property =
                areas.property_sets()[property_set_name].properties().
                    collection<lue::same_shape::constant_shape::Properties>()
                        [discharge_property_name];
            auto& discretization_property =
                areas.collection_property_sets()[collection_property_set_name].
                    properties().collection<
                        lue::same_shape::constant_shape::Properties>()
                            [discretization_property_name];
            discharge_property.set_time_discretisation(
                lue::TimeDiscretization::regular_grid,
                discretization_property);
        }
    }

    // Open and read
    {
        // Phenomenon
        auto& areas = dataset().phenomena()[phenomenon_name];

        // Discharge property --------------------------------------------------
        {
            // Per time box:
            //     Per outlet:
            //         Per time step:
            //             Property value with discharge
            // Per outlet:
            //     Space point

            // Property set
            auto& outlet_points = areas.property_sets()[property_set_name];

            // IDs
            // TODO
            {
                /* auto& object_tracker = */ outlet_points.object_tracker();
                // ...
            }

            // Time domain
            {
                BOOST_CHECK(outlet_points.has_time_domain());

                auto& time_domain = outlet_points.time_domain();
                BOOST_REQUIRE_EQUAL(
                    time_domain.configuration(), time_configuration);

                auto& clock = time_domain.clock();
                BOOST_REQUIRE_EQUAL(time_domain.clock(), clock);

                auto value = time_domain.value<lue::TimeBox>();

                BOOST_REQUIRE_EQUAL(
                    value.memory_datatype(),
                    time_coordinate_datatype);
                BOOST_REQUIRE_EQUAL(value.array_shape(), lue::hdf5::Shape{2});
                BOOST_REQUIRE_EQUAL(value.nr_arrays(), nr_time_boxes);

                std::vector<TimeCoordinateValueType> time_boxes_read(
                    nr_time_boxes * 2);
                value.read(time_boxes_read.data());

                BOOST_CHECK_EQUAL_COLLECTIONS(
                    time_boxes_read.begin(), time_boxes_read.end(),
                    time_boxes.begin(), time_boxes.end());
            }

            // Space domain
            {
                BOOST_REQUIRE(outlet_points.has_space_domain());

                auto& space_domain = outlet_points.space_domain();
                BOOST_REQUIRE_EQUAL(
                    space_domain.configuration(), space_configuration);

                auto value = space_domain.value<lue::StationarySpacePoint>();

                BOOST_REQUIRE_EQUAL(
                    value.memory_datatype(), space_coordinate_datatype);
                BOOST_REQUIRE_EQUAL(
                    value.array_shape(), lue::hdf5::Shape{rank * 1});
                BOOST_REQUIRE_EQUAL(value.nr_arrays(), nr_areas);

                std::vector<SpaceCoordinateValueType> space_points_read(
                    nr_areas * rank * 1);
                value.read(space_points_read.data());

                BOOST_CHECK_EQUAL_COLLECTIONS(
                    space_points_read.begin(), space_points_read.end(),
                    space_points.begin(), space_points.end());
            }

            // Property
            {
                BOOST_REQUIRE(
                    outlet_points.properties().contains(discharge_property_name));
                BOOST_REQUIRE_EQUAL(
                    outlet_points.properties().shape_per_object(
                        discharge_property_name),
                    lue::ShapePerObject::same);
                BOOST_REQUIRE_EQUAL(
                    outlet_points.properties().value_variability(
                        discharge_property_name),
                    lue::ValueVariability::variable);
                BOOST_REQUIRE_EQUAL(
                    outlet_points.properties().shape_variability(
                        discharge_property_name),
                    lue::ShapeVariability::constant);

                auto& properties =
                    outlet_points.properties().collection<
                        lue::same_shape::constant_shape::Properties>();
                auto& discharge_property = properties[discharge_property_name];
                auto& value = discharge_property.value();

                BOOST_REQUIRE_EQUAL(value.nr_arrays(), nr_areas * nr_time_boxes);
                BOOST_REQUIRE_EQUAL(value.memory_datatype(), discharge_datatype);
                BOOST_REQUIRE_EQUAL(value.array_shape(), lue::hdf5::Shape{});

                std::vector<DischargeValueType> values_read(
                    nr_areas * nr_time_boxes);

                value.read(values_read.data());

                BOOST_CHECK_EQUAL_COLLECTIONS(
                        values_read.begin(), values_read.end(),
                        values.begin(), values.end());

                BOOST_CHECK(discharge_property.time_is_discretized());
                BOOST_CHECK(!discharge_property.space_is_discretized());
            }
        }

        // Discretization property ---------------------------------------------
        // TODO Find the property through the discharge property!
        {
            // Per time box:
            //     Property value with number of time steps

            auto& discharge_property =
                areas.property_sets()[property_set_name].properties().
                    collection<lue::same_shape::constant_shape::Properties>()
                        [discharge_property_name];
            auto discretization_type =
                discharge_property.time_discretization_type();
            BOOST_REQUIRE_EQUAL(
                discretization_type,
                lue::TimeDiscretization::regular_grid);

            // Property set
            auto property = discharge_property.time_discretization_property();
            lue::PropertySet property_set{property.property_set_group()};

            // IDs
            // TODO
            // Only a single object!
            {
                /* auto& object_tracker = */ property_set.object_tracker();
                // ...
            }

            // Time domain
            {
                // Above, we are passing in the time domain of
                // the discharge property to the property set of the
                // discretization property. This time domain is linked
                // from the latter property set. This results in only
                // a single time domain being written to file, so by
                // definition, this means that the time domains are equal.
                // Here, we test the contents anyway, to be sure.
                BOOST_CHECK(property_set.has_time_domain());

                auto& time_domain = property_set.time_domain();
                BOOST_REQUIRE_EQUAL(
                    time_domain.configuration(), time_configuration);

                auto& clock = time_domain.clock();
                BOOST_REQUIRE_EQUAL(time_domain.clock(), clock);

                auto value = time_domain.value<lue::TimeBox>();

                BOOST_REQUIRE_EQUAL(
                    value.memory_datatype(),
                    time_coordinate_datatype);
                BOOST_REQUIRE_EQUAL(value.array_shape(), lue::hdf5::Shape{2});
                BOOST_REQUIRE_EQUAL(value.nr_arrays(), nr_time_boxes);

                std::vector<TimeCoordinateValueType> time_boxes_read(
                    nr_time_boxes * 2);
                value.read(time_boxes_read.data());

                BOOST_CHECK_EQUAL_COLLECTIONS(
                    time_boxes_read.begin(), time_boxes_read.end(),
                    time_boxes.begin(), time_boxes.end());
            }

            // Space domain
            {
                BOOST_CHECK(!property_set.has_space_domain());
            }

            // Property
            {
                BOOST_REQUIRE(
                    property_set.properties().contains(
                        discretization_property_name));
                BOOST_REQUIRE_EQUAL(
                    property_set.properties().shape_per_object(
                        discretization_property_name),
                    lue::ShapePerObject::same);
                BOOST_REQUIRE_EQUAL(
                    property_set.properties().value_variability(
                        discretization_property_name),
                    lue::ValueVariability::variable);
                BOOST_REQUIRE_EQUAL(
                    property_set.properties().shape_variability(
                        discretization_property_name),
                    lue::ShapeVariability::constant);

                auto& properties =
                    property_set.properties().collection<
                        lue::same_shape::constant_shape::Properties>();
                auto& discretization_property =
                    properties[discretization_property_name];
                auto& value = discretization_property.value();

                BOOST_REQUIRE_EQUAL(value.nr_arrays(), nr_time_boxes);
                BOOST_REQUIRE_EQUAL(value.memory_datatype(), shape_datatype);
                BOOST_REQUIRE_EQUAL(value.array_shape(), lue::hdf5::Shape{1});

                std::vector<lue::hdf5::Shape::value_type> shapes_read(
                    nr_time_boxes);

                value.read(shapes_read.data());

                BOOST_CHECK_EQUAL_COLLECTIONS(
                        shapes_read.begin(), shapes_read.end(),
                        shapes.begin(), shapes.end());

                BOOST_CHECK(!discretization_property.time_is_discretized());
                BOOST_CHECK(!discretization_property.space_is_discretized());
            }
        }
    }
}
