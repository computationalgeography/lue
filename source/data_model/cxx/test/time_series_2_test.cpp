#define BOOST_TEST_MODULE lue time_series 2
#include "lue/test/algorithm.hpp"
#include "lue/test/dataset_fixture.hpp"
#include "lue/test/stream.hpp"
#include "lue/hdf5/test/stream.hpp"
#include "lue/validate.hpp"
#include <boost/test/included/unit_test.hpp>


BOOST_FIXTURE_TEST_CASE(create, lue::data_model::DatasetFixture)
{
    // Time series as implemented here:
    // - Discharge at catchment outlets
    //     - Each catchment is located at a fixed point in space
    //     - Variable number of outlets per time cell
    //     - Discretized within multiple time boxes
    //
    // So, presence is discretized and all temporal information within
    // the same property-set is stored per time cell, including the
    // objects which are part of the active set.

    std::string const phenomenon_name = "areas";
    std::string const property_set_name = "outlets";

    lue::data_model::Count const max_active_set_size = 10;
    lue::data_model::Count const nr_time_boxes = 3;

    // Counts per box
    std::vector<lue::data_model::Count> count(nr_time_boxes);
    lue::data_model::generate_random_counts(count, 0, 100);
    auto const nr_time_cells = std::accumulate(count.begin(), count.end(), lue::data_model::Count{0});

    // Object tracking info for information that changes through time
    std::vector<lue::data_model::Count> active_set_sizes(nr_time_cells);
    lue::data_model::generate_random_counts(active_set_sizes, 0, max_active_set_size);
    std::vector<lue::data_model::Index> active_set_idxs(nr_time_cells);
    std::vector<lue::data_model::ID> active_ids(
        std::accumulate(active_set_sizes.begin(), active_set_sizes.end(), std::size_t{0}));
    lue::data_model::select_random_ids(active_set_sizes, active_set_idxs, active_ids, max_active_set_size);

    // Object IDs for information that does not change through time
    std::vector<lue::data_model::ID> unique_active_ids(active_ids);
    std::sort(unique_active_ids.begin(), unique_active_ids.end());
    unique_active_ids.erase(
        std::unique(unique_active_ids.begin(), unique_active_ids.end()), unique_active_ids.end());
    auto const nr_unique_active_objects = unique_active_ids.size();

    // Time domain
    // Boxes in time
    lue::data_model::TimeConfiguration time_configuration{lue::data_model::TimeDomainItemType::cell};
    lue::data_model::Clock const clock{lue::data_model::time::Unit::day, 1};
    using TimeCoordinateValueType = lue::data_model::time::TickPeriodCount;
    lue::hdf5::Datatype const time_coordinate_datatype{
        lue::hdf5::NativeDatatypeTraits<TimeCoordinateValueType>::type_id()};
    std::vector<TimeCoordinateValueType> time_boxes(nr_time_boxes * 2);
    lue::data_model::generate_random_strictly_increasing_values(time_boxes, 0, 1000);


    // Space domain
    // Points in space that don't change location:
    //     constant shape x constant value
    lue::data_model::SpaceConfiguration space_configuration{
        lue::data_model::Mobility::stationary, lue::data_model::SpaceDomainItemType::point};
    using SpaceCoordinateValueType = double;
    lue::hdf5::Datatype const space_coordinate_datatype{
        lue::hdf5::NativeDatatypeTraits<SpaceCoordinateValueType>::type_id()};
    std::size_t const rank = 2;
    std::vector<SpaceCoordinateValueType> space_points(nr_unique_active_objects * rank);
    lue::data_model::generate_random_values(space_points, 0, 1000);

    // Discharge property
    // Amount of discharge per active object in an active set per time cell
    std::string const discharge_property_name = "discharge";
    using DischargeValueType = float;
    lue::hdf5::Datatype const discharge_datatype{
        lue::hdf5::NativeDatatypeTraits<DischargeValueType>::type_id()};
    std::vector<std::vector<DischargeValueType>> discharge_values(nr_time_cells);
    for (size_t t = 0; t < nr_time_cells; ++t)
    {
        discharge_values[t].resize(active_set_sizes[t]);
    }
    lue::data_model::generate_random_values(discharge_values, 0.0, 1500.0);

    // Create and write
    {
        // Phenomenon
        auto& areas = dataset().add_phenomenon(phenomenon_name);

        areas.object_id().expand(nr_unique_active_objects);
        areas.object_id().write(unique_active_ids.data());

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
                active_set_index.expand(nr_time_cells);

                auto& active_object_id = object_tracker.active_object_id();
                active_object_id.expand(active_ids.size());

                for (std::size_t s = 0, active_set_idx = 0; s < nr_time_cells; ++s)
                {

                    auto const active_set_size = active_set_sizes[s];

                    // Write index of this active set
                    active_set_index.write(s, &active_set_idxs[s]);

                    // Write IDs of the objects in this active set
                    active_object_id.write(
                        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
                        {active_set_idx, active_set_idx + active_set_size},
                        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
                        active_ids.data() + active_set_idx);

                    active_set_idx += active_set_size;
                }
            }

            // Time domain
            // Time boxes and for each box a count
            {
                auto& time_domain = outlet_points.time_domain();
                auto value = time_domain.value<lue::data_model::TimeCell>();
                value.expand(nr_time_boxes);
                value.write(time_boxes.data());

                auto& counts_ = value.count();
                assert(count.size() == nr_time_boxes);
                counts_.expand(nr_time_boxes);
                counts_.write(count.data());
            }

            // Space domain
            // For each object that is ever active a point
            {
                auto& space_domain = outlet_points.space_domain();
                auto value = space_domain.value<lue::data_model::StationarySpacePoint>();
                value.expand(nr_unique_active_objects);
                value.write(space_points.data());
            }

            // Property
            {
                using Property = lue::data_model::same_shape::constant_shape::Property;
                auto& properties = outlet_points.properties();
                auto& discharge_property =
                    properties.add<Property>(discharge_property_name, discharge_datatype);

                auto& value = discharge_property.value();
                for (std::size_t t = 0; t < nr_time_cells; ++t)
                {
                    lue::data_model::IndexRange index_range{
                        value.nr_arrays(), value.nr_arrays() + discharge_values[t].size()};
                    value.expand(index_range.size());
                    value.write(index_range, discharge_values[t].data());
                }
            }
        }
    }

    lue::data_model::assert_is_valid(pathname());

    // Open and read
    {
        // Phenomenon
        auto& areas = dataset().phenomena()[phenomenon_name];

        BOOST_REQUIRE_EQUAL(areas.object_id().nr_objects(), nr_unique_active_objects);
        std::vector<lue::data_model::ID> ids_read(nr_unique_active_objects);
        areas.object_id().read(ids_read.data());
        BOOST_CHECK_EQUAL_COLLECTIONS(
            ids_read.begin(), ids_read.end(), unique_active_ids.begin(), unique_active_ids.end());

        // Discharge property --------------------------------------------------
        {
            // Property set
            auto& outlet_points = areas.property_sets()[property_set_name];

            // IDs
            {
                auto& object_tracker = outlet_points.object_tracker();

                auto& active_set_index = object_tracker.active_set_index();
                BOOST_REQUIRE_EQUAL(active_set_index.nr_arrays(), nr_time_cells);
                std::vector<lue::data_model::Index> active_set_idxs_read(nr_time_cells);
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

                BOOST_CHECK_EQUAL(object_tracker.active_object_index().nr_arrays(), 0);
            }


            // Time domain
            {
                BOOST_CHECK(outlet_points.has_time_domain());

                auto& time_domain = outlet_points.time_domain();
                BOOST_REQUIRE_EQUAL(time_domain.configuration(), time_configuration);

                auto const& clock = time_domain.clock();
                BOOST_REQUIRE_EQUAL(time_domain.clock(), clock);

                auto value = time_domain.value<lue::data_model::TimeCell>();

                BOOST_REQUIRE_EQUAL(value.memory_datatype(), time_coordinate_datatype);
                BOOST_REQUIRE_EQUAL(value.array_shape(), lue::hdf5::Shape{2});
                BOOST_REQUIRE_EQUAL(value.nr_arrays(), nr_time_boxes);
                BOOST_REQUIRE_EQUAL(value.nr_arrays(), value.nr_counts());

                std::vector<lue::data_model::Count> counts_read(value.nr_counts());
                value.count().read(counts_read.data());

                BOOST_CHECK_EQUAL_COLLECTIONS(
                    counts_read.begin(), counts_read.end(), count.begin(), count.end());

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
                BOOST_REQUIRE_EQUAL(value.nr_arrays(), nr_unique_active_objects);

                std::vector<SpaceCoordinateValueType> space_points_read(nr_unique_active_objects * rank * 1);
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
                    lue::data_model::ShapeVariability::constant);

                auto& properties = outlet_points.properties()
                                       .collection<lue::data_model::same_shape::constant_shape::Properties>();
                auto& discharge_property = properties[discharge_property_name];
                auto& value = discharge_property.value();

                BOOST_REQUIRE_EQUAL(value.nr_arrays(), active_ids.size());
                BOOST_REQUIRE_EQUAL(value.array_shape(), lue::hdf5::Shape{});
                BOOST_REQUIRE_EQUAL(value.memory_datatype(), discharge_datatype);

                {
                    lue::data_model::Index begin{};
                    lue::data_model::Index end = 0;

                    for (std::size_t t = 0; t < nr_time_cells; ++t)
                    {
                        begin = end;
                        end = begin + active_set_sizes[t];
                        lue::data_model::IndexRange index_range{begin, end};

                        std::vector<DischargeValueType> discharge_values_read(active_set_sizes[t]);
                        value.read(index_range, discharge_values_read.data());

                        BOOST_CHECK_EQUAL_COLLECTIONS(
                            discharge_values_read.begin(),
                            discharge_values_read.end(),
                            discharge_values[t].begin(),
                            discharge_values[t].end());
                    }
                }

                BOOST_CHECK(!discharge_property.time_is_discretized());
                BOOST_CHECK(!discharge_property.space_is_discretized());
            }
        }
    }
}
