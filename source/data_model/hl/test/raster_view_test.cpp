#define BOOST_TEST_MODULE lue data_model_hl
#include "lue/data_model/hl/raster_view.hpp"
#include "lue/validate.hpp"
#include "lue/test/print.hpp"
#include <boost/test/unit_test.hpp>


BOOST_AUTO_TEST_CASE(use_case_1)
{
    std::string const dataset_name{"use_case_1"};
    std::string const dataset_pathname{"hl/" + dataset_name};
    std::string const phenomenon_name{"area"};
    std::string const property_set_name{"area"};

    lue::Clock const clock{lue::time::Unit::day, 1};
    lue::Count nr_time_steps{50};
    lue::Count nr_rows{60};
    lue::Count nr_cols{40};
    lue::hdf5::Shape raster_shape{nr_rows, nr_cols};

    // -------------------------------------------------------------------------
    auto dataset_ptr = std::make_shared<lue::Dataset>(
        lue::create_in_memory_dataset(dataset_pathname));
    lue::RasterView view =
        lue::create_raster_view(
            dataset_ptr,
            phenomenon_name, property_set_name,
            clock, nr_time_steps, {10, 10 + nr_time_steps},
            raster_shape, {0, 0, 400, 600});

    // Details
    {
        lue::Dataset const& dataset{*dataset_ptr};
        lue::assert_is_valid(dataset);

        BOOST_CHECK_EQUAL(dataset.universes().size(), 0);
        BOOST_CHECK_EQUAL(dataset.phenomena().size(), 1);
        BOOST_REQUIRE(dataset.phenomena().contains(phenomenon_name));

        auto const& phenomenon{dataset.phenomena()[phenomenon_name]};

        auto const& object_id{phenomenon.object_id()};
        BOOST_REQUIRE_EQUAL(object_id.nr_objects(), 1);

        BOOST_CHECK(phenomenon.collection_property_sets().empty());

        BOOST_REQUIRE_EQUAL(phenomenon.property_sets().size(), 1);
        BOOST_REQUIRE(phenomenon.property_sets().contains(property_set_name));

        auto const& property_set{phenomenon.property_sets()[property_set_name]};
        auto const& properties{property_set.properties()};

        BOOST_CHECK_EQUAL(properties.size(), 2);

        // Time domain
        {
            // Single time box
            BOOST_REQUIRE(property_set.has_time_domain());

            lue::TimeDomain const& time_domain{property_set.time_domain()};
            lue::TimeConfiguration const& configuration{
                time_domain.configuration()};

            BOOST_REQUIRE_EQUAL(
                configuration.value<lue::TimeDomainItemType>(),
                lue::TimeDomainItemType::box);

            BOOST_CHECK(time_domain.clock() == clock);

            lue::TimeBox value{const_cast<lue::TimeDomain&>(time_domain)
                .value<lue::TimeBox>()};

            BOOST_REQUIRE_EQUAL(value.nr_locations(), 1);
            BOOST_REQUIRE_EQUAL(
                value.file_datatype(), lue::hdf5::std_uint64_le);
            BOOST_REQUIRE_EQUAL(value.array_shape(), (lue::hdf5::Shape{2}));

            std::vector<lue::time::DurationCount> durations(2);
            value.read(0, durations.data());

            BOOST_CHECK_EQUAL(durations[0], 10);
            BOOST_CHECK_EQUAL(durations[1], 10 + nr_time_steps);

            // Discretization property
            BOOST_REQUIRE(properties.contains("time_grid_shape"));
        }

        // Space domain
        {
            // Single space box
            BOOST_REQUIRE(property_set.has_space_domain());

            auto const& space_domain{property_set.space_domain()};

            BOOST_CHECK(!space_domain.presence_is_discretized());

            lue::SpaceConfiguration const& configuration{
                space_domain.configuration()};

            BOOST_REQUIRE_EQUAL(
                configuration.value<lue::Mobility>(),
                lue::Mobility::stationary);
            BOOST_REQUIRE_EQUAL(
                configuration.value<lue::SpaceDomainItemType>(),
                lue::SpaceDomainItemType::box);

            lue::StationarySpaceBox value{
                const_cast<lue::SpaceDomain&>(space_domain)
                    .value<lue::StationarySpaceBox>()};

            BOOST_REQUIRE_EQUAL(value.nr_boxes(), 1);
            BOOST_REQUIRE_EQUAL(
                value.file_datatype(), lue::hdf5::ieee_float64_le);

            BOOST_REQUIRE_EQUAL(
                value.array_shape(), (lue::hdf5::Shape{4}));

            std::vector<double> box(4);
            value.read(0, box.data());

            BOOST_CHECK_EQUAL(box[0], 0);
            BOOST_CHECK_EQUAL(box[1], 0);
            BOOST_CHECK_EQUAL(box[2], 400);
            BOOST_CHECK_EQUAL(box[3], 600);

            // Discretization property
            BOOST_REQUIRE(properties.contains("space_grid_shape"));
        }
    }

    // High-level API
    {
        BOOST_CHECK_EQUAL(view.phenomenon_name(), phenomenon_name);
        BOOST_CHECK_EQUAL(view.property_set_name(), property_set_name);

        BOOST_CHECK(
            view.time_box() ==
            (lue::RasterView::TimeBox{10, 10 + nr_time_steps}));
        BOOST_CHECK_EQUAL(view.nr_time_steps(), nr_time_steps);

        BOOST_CHECK(
            view.space_box() == (lue::RasterView::SpaceBox{0, 0, 400, 600}));
        auto const& raster_shape{view.raster_shape()};
        BOOST_REQUIRE_EQUAL(raster_shape.size(), 2);
        BOOST_CHECK_EQUAL(raster_shape[0], nr_rows);
        BOOST_CHECK_EQUAL(raster_shape[1], nr_cols);

        BOOST_CHECK_EQUAL(view.nr_layers(), 0);

        {
            BOOST_CHECK(!view.contains("elevation"));
            lue::RasterView::Layer layer{
                view.add_layer<double>("elevation")};
            BOOST_CHECK(view.contains("elevation"));

            // layer.write(time_step_idx, elevation_data);
        }

        {
            BOOST_CHECK(!view.contains("soil"));
            lue::RasterView::Layer layer{
                view.add_layer<std::uint32_t>("soil")};
            BOOST_CHECK(view.contains("soil"));

            // layer.write(time_step_idx, soil_data);
        }

        BOOST_CHECK_EQUAL(view.nr_layers(), 2);
    }

    {
        // Open a second view on the just created dataset
        lue::RasterView view{dataset_ptr, phenomenon_name, property_set_name};

        BOOST_CHECK_EQUAL(view.nr_layers(), 2);
        BOOST_CHECK(view.contains("elevation"));
        BOOST_CHECK(view.contains("soil"));
    }
}
