#define BOOST_TEST_MODULE lue time_series 1
#include "lue/test.hpp"
#include <boost/test/unit_test.hpp>


BOOST_FIXTURE_TEST_CASE(create, lue::data_model::test::DatasetFixture)
{
    // Time series as implemented here:
    // - Discharge at catchment outlets
    //     - Located at fixed points in space
    //     - Variable number of outlets per time box
    //     - Discretized within multiple time boxes
    std::string const phenomenon_name = "areas";
    std::string const property_set_name = "outlets";
    std::string const collection_property_set_name = "outlets_collection";

    lue::data_model::Count const nr_areas = 10;
    lue::data_model::Count const nr_time_boxes = 3;

    // IDs
    // Object IDs for information that does not change through time
    std::vector<lue::data_model::ID> ids(nr_areas);
    lue::data_model::test::generate_random_ids(ids);
    lue::data_model::ID const collection_id = 5;

    // Object tracking info for information that changes through time
    std::vector<lue::data_model::Count> active_set_sizes(nr_time_boxes);
    lue::data_model::test::generate_random_counts(active_set_sizes, 0, nr_areas);
    std::vector<lue::data_model::Index> active_set_idxs(nr_time_boxes);
    std::vector<lue::data_model::ID> active_ids(
        std::accumulate(active_set_sizes.begin(), active_set_sizes.end(), std::size_t{0}));
    lue::data_model::test::select_random_ids(active_set_sizes, active_set_idxs, active_ids, nr_areas);

    // Time domain
    // Boxes in time
    lue::data_model::TimeConfiguration time_configuration{lue::data_model::TimeDomainItemType::box};
    lue::data_model::Clock const clock{lue::data_model::time::Unit::day, 1};
    using TimeCoordinateValueType = lue::data_model::time::TickPeriodCount;
    lue::hdf5::Datatype const time_coordinate_datatype{
        lue::hdf5::NativeDatatypeTraits<TimeCoordinateValueType>::type_id()};
    std::vector<TimeCoordinateValueType> time_boxes(nr_time_boxes * 2);
    lue::data_model::test::generate_random_strictly_increasing_values(time_boxes, 0, 1000);

    // Space domain
    // Points in space that don't change location:
    //     constant shape x constant value
    lue::data_model::SpaceConfiguration space_configuration{
        lue::data_model::Mobility::stationary, lue::data_model::SpaceDomainItemType::point};
    using SpaceCoordinateValueType = double;
    lue::hdf5::Datatype const space_coordinate_datatype{
        lue::hdf5::NativeDatatypeTraits<SpaceCoordinateValueType>::type_id()};
    std::size_t const rank = 2;
    std::vector<SpaceCoordinateValueType> space_points(nr_areas * rank);
    lue::data_model::test::generate_random_values(space_points, 0, 1000);

    // Discretization property
    // Number of time steps per time box.
    std::string const discretization_property_name = "discretization";
    lue::hdf5::Datatype const shape_datatype{
        lue::hdf5::NativeDatatypeTraits<lue::hdf5::Shape::value_type>::type_id()};
    std::vector<lue::hdf5::Shape::value_type> shapes(nr_time_boxes);
    lue::data_model::test::generate_random_values(shapes, 10, 20);

    // Discharge property
    // Amount of discharge per active object and per time box. This
    // amount is discretized in time steps.
    std::string const discharge_property_name = "discharge";
    using DischargeValueType = float;
    lue::hdf5::Datatype const discharge_datatype{
        lue::hdf5::NativeDatatypeTraits<DischargeValueType>::type_id()};
    std::vector<std::vector<DischargeValueType>> discharge_values(nr_time_boxes);
    for (size_t t = 0; t < nr_time_boxes; ++t)
    {
        discharge_values[t].resize(shapes[t] * active_set_sizes[t]);
    }
    lue::data_model::test::generate_random_values(discharge_values, 0.0, 1500.0);

    // Create and write
    {
        // Phenomenon
        auto& areas = dataset().add_phenomenon(phenomenon_name);

        areas.object_id().expand(nr_areas);
        areas.object_id().write(ids.data());

        // Discharge property --------------------------------------------------
        {
            // Property set
            auto& outlet_points = areas.property_sets().add(
                property_set_name,
                time_configuration,
                clock,
                space_configuration,
                space_coordinate_datatype,
                rank);

            // IDs
            {
                auto& object_tracker = outlet_points.object_tracker();

                auto& active_set_index = object_tracker.active_set_index();
                active_set_index.expand(nr_time_boxes);

                auto& active_object_id = object_tracker.active_object_id();
                active_object_id.expand(active_ids.size());

                for (std::size_t s = 0, active_set_idx = 0; s < nr_time_boxes; ++s)
                {

                    auto const active_set_size = active_set_sizes[s];

                    // Write index of this active set
                    active_set_index.write(s, &active_set_idxs[s]);

                    // Write IDs of the objects in this active set
                    active_object_id.write(
                        {active_set_idx, active_set_idx + active_set_size},
                        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
                        active_ids.data() + active_set_idx);

                    active_set_idx += active_set_size;
                }
            }

            // Time domain
            {
                auto& time_domain = outlet_points.time_domain();
                auto value = time_domain.value<lue::data_model::TimeBox>();
                value.expand(nr_time_boxes);
                value.write(time_boxes.data());
            }

            // Space domain
            {
                auto& space_domain = outlet_points.space_domain();
                auto value = space_domain.value<lue::data_model::StationarySpacePoint>();
                value.expand(nr_areas);
                value.write(space_points.data());
            }

            // Property
            {
                using Property = lue::data_model::same_shape::variable_shape::Property;
                auto& properties = outlet_points.properties();
                auto& discharge_property =
                    properties.add<Property>(discharge_property_name, discharge_datatype, 1);

                for (std::size_t t = 0; t < nr_time_boxes; ++t)
                {
                    auto& value = discharge_property.value();
                    auto value_t = value.expand(t, active_set_sizes[t], {shapes[t]});
                    value_t.write(discharge_values[t].data());
                }
            }
        }

        // Discretization property ---------------------------------------------
        {
            // Property set
            auto& collection = areas.collection_property_sets().add(
                collection_property_set_name,
                // Time domain from discharge property
                areas.property_sets()[property_set_name].time_domain());

            // IDs
            {
                // For each time box we have to store the IDs of the
                // active objects. Here there is only one such object:
                // the collection as a whole. Just store the same number.
                // This could be optimized once we explicitly support
                // constant collections of active objects. In that case we
                // only need to store an ID once instead of for each
                // location in time.
                auto& object_tracker = collection.object_tracker();
                auto& active_set_index = object_tracker.active_set_index();
                active_set_index.expand(nr_time_boxes);
                auto& active_object_id = object_tracker.active_object_id();
                active_object_id.expand(nr_time_boxes);

                lue::data_model::Index active_set_idx = 0;
                for (std::size_t s = 0; s < nr_time_boxes; ++s)
                {

                    auto const active_set_size = 1;

                    // Write index of this active set
                    active_set_index.write(s, &active_set_idx);

                    // Write IDs of the objects in this active set
                    active_object_id.write(
                        {active_set_idx, active_set_idx + active_set_size}, &collection_id);

                    active_set_idx += active_set_size;
                }
            }

            // Property
            {
                using Property = lue::data_model::same_shape::constant_shape::Property;
                auto& properties = collection.properties();
                auto& discretization_property = properties.add<Property>(
                    discretization_property_name, shape_datatype, lue::hdf5::Shape{1});  // nr time steps
                auto& value = discretization_property.value();
                value.expand(nr_time_boxes);
                value.write(shapes.data());
            }
        }

        // Link from discharge property to discretization property
        {
            auto& discharge_property =
                areas.property_sets()[property_set_name]
                    .properties()
                    .collection<
                        lue::data_model::same_shape::variable_shape::Properties>()[discharge_property_name];
            auto& discretization_property =
                areas.collection_property_sets()[collection_property_set_name]
                    .properties()
                    .collection<lue::data_model::same_shape::constant_shape::Properties>()
                        [discretization_property_name];
            discharge_property.set_time_discretization(
                lue::data_model::TimeDiscretization::regular_grid, discretization_property);
        }
    }

    lue::data_model::assert_is_valid(pathname());

    // Open and read
    {
        // Phenomenon
        auto& areas = dataset().phenomena()[phenomenon_name];

        BOOST_REQUIRE_EQUAL(areas.object_id().nr_objects(), nr_areas);
        std::vector<lue::data_model::ID> ids_read(nr_areas);
        areas.object_id().read(ids_read.data());
        BOOST_CHECK_EQUAL_COLLECTIONS(ids_read.begin(), ids_read.end(), ids.begin(), ids.end());

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
            {
                auto& object_tracker = outlet_points.object_tracker();

                auto& active_set_index = object_tracker.active_set_index();
                BOOST_REQUIRE_EQUAL(active_set_index.nr_arrays(), nr_time_boxes);
                std::vector<lue::data_model::Index> active_set_idxs_read(nr_time_boxes);
                active_set_index.read(active_set_idxs_read.data());
                BOOST_CHECK_EQUAL_COLLECTIONS(
                    active_set_idxs_read.begin(),
                    active_set_idxs_read.end(),
                    active_set_idxs.begin(),
                    active_set_idxs.end());

                auto& active_object_id = object_tracker.active_object_id();
                BOOST_REQUIRE_EQUAL(active_object_id.nr_arrays(), active_ids.size());
                std::vector<lue::data_model::ID> active_ids_read(active_ids.size());
                active_object_id.read(active_ids_read.data());
                BOOST_CHECK_EQUAL_COLLECTIONS(
                    active_ids_read.begin(), active_ids_read.end(), active_ids.begin(), active_ids.end());
            }

            // Time domain
            {
                BOOST_CHECK(outlet_points.has_time_domain());

                auto& time_domain = outlet_points.time_domain();
                BOOST_REQUIRE_EQUAL(time_domain.configuration(), time_configuration);

                auto const& clock = time_domain.clock();
                BOOST_REQUIRE_EQUAL(time_domain.clock(), clock);

                auto value = time_domain.value<lue::data_model::TimeBox>();

                BOOST_REQUIRE_EQUAL(value.memory_datatype(), time_coordinate_datatype);
                BOOST_REQUIRE_EQUAL(value.array_shape(), lue::hdf5::Shape{2});
                BOOST_REQUIRE_EQUAL(value.nr_arrays(), nr_time_boxes);

                std::vector<TimeCoordinateValueType> time_boxes_read(nr_time_boxes * 2);
                value.read(time_boxes_read.data());

                BOOST_CHECK_EQUAL_COLLECTIONS(
                    time_boxes_read.begin(), time_boxes_read.end(), time_boxes.begin(), time_boxes.end());
            }

            // Space domain
            {
                BOOST_REQUIRE(outlet_points.has_space_domain());

                auto& space_domain = outlet_points.space_domain();
                BOOST_REQUIRE_EQUAL(space_domain.configuration(), space_configuration);

                auto value = space_domain.value<lue::data_model::StationarySpacePoint>();

                BOOST_REQUIRE_EQUAL(value.memory_datatype(), space_coordinate_datatype);
                BOOST_REQUIRE_EQUAL(value.array_shape(), lue::hdf5::Shape{rank * 1});
                BOOST_REQUIRE_EQUAL(value.nr_arrays(), nr_areas);

                std::vector<SpaceCoordinateValueType> space_points_read(nr_areas * rank * 1);
                value.read(space_points_read.data());

                BOOST_CHECK_EQUAL_COLLECTIONS(
                    space_points_read.begin(),
                    space_points_read.end(),
                    space_points.begin(),
                    space_points.end());
            }

            // Property
            {
                BOOST_REQUIRE(outlet_points.properties().contains(discharge_property_name));
                BOOST_REQUIRE_EQUAL(
                    outlet_points.properties().shape_per_object(discharge_property_name),
                    lue::data_model::ShapePerObject::same);
                BOOST_REQUIRE_EQUAL(
                    outlet_points.properties().value_variability(discharge_property_name),
                    lue::data_model::ValueVariability::variable);
                BOOST_REQUIRE_EQUAL(
                    outlet_points.properties().shape_variability(discharge_property_name),
                    lue::data_model::ShapeVariability::variable);

                auto& properties = outlet_points.properties()
                                       .collection<lue::data_model::same_shape::variable_shape::Properties>();
                auto& discharge_property = properties[discharge_property_name];
                auto& value = discharge_property.value();

                BOOST_REQUIRE_EQUAL(value.nr_locations_in_time(), nr_time_boxes);
                BOOST_REQUIRE_EQUAL(value.memory_datatype(), discharge_datatype);

                for (std::size_t t = 0; t < nr_time_boxes; ++t)
                {
                    auto value_t = value[t];

                    BOOST_REQUIRE_EQUAL(value_t.nr_arrays(), active_set_sizes[t]);
                    BOOST_REQUIRE_EQUAL(value_t.array_shape(), lue::hdf5::Shape{shapes[t]});

                    std::vector<DischargeValueType> discharge_values_read(active_set_sizes[t] * shapes[t]);
                    value_t.read(discharge_values_read.data());

                    BOOST_CHECK_EQUAL_COLLECTIONS(
                        discharge_values_read.begin(),
                        discharge_values_read.end(),
                        discharge_values[t].begin(),
                        discharge_values[t].end());
                }

                BOOST_CHECK(discharge_property.time_is_discretized());
                BOOST_CHECK(!discharge_property.space_is_discretized());
            }
        }

        // Discretization property ---------------------------------------------
        {
            // Per time box:
            //     Property value with number of time steps

            auto& discharge_property =
                areas.property_sets()[property_set_name]
                    .properties()
                    .collection<
                        lue::data_model::same_shape::variable_shape::Properties>()[discharge_property_name];
            BOOST_REQUIRE_EQUAL(
                discharge_property.time_discretization_type(),
                lue::data_model::TimeDiscretization::regular_grid);

            // Property set
            auto property = discharge_property.time_discretization_property();
            auto property_set{lue::data_model::property_set(property)};

            // IDs
            {
                auto& object_tracker = property_set.object_tracker();

                auto& active_set_index = object_tracker.active_set_index();
                BOOST_REQUIRE_EQUAL(active_set_index.nr_arrays(), nr_time_boxes);
                std::vector<lue::data_model::Index> active_set_idxs(nr_time_boxes);
                std::iota(active_set_idxs.begin(), active_set_idxs.end(), 0);
                std::vector<lue::data_model::Index> active_set_idxs_read(nr_time_boxes);
                active_set_index.read(active_set_idxs_read.data());
                BOOST_CHECK_EQUAL_COLLECTIONS(
                    active_set_idxs_read.begin(),
                    active_set_idxs_read.end(),
                    active_set_idxs.begin(),
                    active_set_idxs.end());

                auto& active_object_id = object_tracker.active_object_id();
                BOOST_REQUIRE_EQUAL(active_object_id.nr_arrays(), nr_time_boxes);
                std::vector<lue::data_model::ID> active_ids_read(nr_time_boxes);
                active_object_id.read(active_ids_read.data());

                BOOST_CHECK(std::all_of(
                    active_ids_read.begin(),
                    active_ids_read.end(),
                    [collection_id](lue::data_model::ID const id) { return id == collection_id; }));
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
                BOOST_REQUIRE_EQUAL(time_domain.configuration(), time_configuration);

                auto const& clock = time_domain.clock();
                BOOST_REQUIRE_EQUAL(time_domain.clock(), clock);

                auto value = time_domain.value<lue::data_model::TimeBox>();

                BOOST_REQUIRE_EQUAL(value.memory_datatype(), time_coordinate_datatype);
                BOOST_REQUIRE_EQUAL(value.array_shape(), lue::hdf5::Shape{2});
                BOOST_REQUIRE_EQUAL(value.nr_arrays(), nr_time_boxes);

                std::vector<TimeCoordinateValueType> time_boxes_read(nr_time_boxes * 2);
                value.read(time_boxes_read.data());

                BOOST_CHECK_EQUAL_COLLECTIONS(
                    time_boxes_read.begin(), time_boxes_read.end(), time_boxes.begin(), time_boxes.end());
            }

            // Space domain
            {
                BOOST_CHECK(!property_set.has_space_domain());
            }

            // Property
            {
                BOOST_REQUIRE(property_set.properties().contains(discretization_property_name));
                BOOST_REQUIRE_EQUAL(
                    property_set.properties().shape_per_object(discretization_property_name),
                    lue::data_model::ShapePerObject::same);
                BOOST_REQUIRE_EQUAL(
                    property_set.properties().value_variability(discretization_property_name),
                    lue::data_model::ValueVariability::variable);
                BOOST_REQUIRE_EQUAL(
                    property_set.properties().shape_variability(discretization_property_name),
                    lue::data_model::ShapeVariability::constant);

                auto& properties = property_set.properties()
                                       .collection<lue::data_model::same_shape::constant_shape::Properties>();
                auto& discretization_property = properties[discretization_property_name];
                auto& value = discretization_property.value();

                BOOST_REQUIRE_EQUAL(value.nr_arrays(), nr_time_boxes);
                BOOST_REQUIRE_EQUAL(value.memory_datatype(), shape_datatype);
                BOOST_REQUIRE_EQUAL(value.array_shape(), lue::hdf5::Shape{1});

                std::vector<lue::hdf5::Shape::value_type> shapes_read(nr_time_boxes);

                value.read(shapes_read.data());

                BOOST_CHECK_EQUAL_COLLECTIONS(
                    shapes_read.begin(), shapes_read.end(), shapes.begin(), shapes.end());

                BOOST_CHECK(!discretization_property.time_is_discretized());
                BOOST_CHECK(!discretization_property.space_is_discretized());
            }
        }
    }
}
