#define BOOST_TEST_MODULE lue hl time_series
#include <boost/test/unit_test.hpp>
#include "lue/hl/time_series.hpp"
#include "lue/test/dataset_fixture.hpp"
#include <algorithm>
#include <numeric>


BOOST_AUTO_TEST_CASE(create_time_series)
{
    using namespace lue;

    std::string const dataset_name = "create_time_series.lue";
    test::DatasetFixture fixture{dataset_name};

    // Time box
    time::TickPeriodCount const nr_units = 1;
    Clock const clock{time::Unit::hour, nr_units};
    time::DurationCount const start_time_point{0};
    time::DurationCount const end_time_point{23};
    hl::TimeSeries::TimeDomain const time_domain{
        clock, start_time_point, end_time_point};

    // Time steps
    std::size_t const nr_steps = 24;
    hl::TimeSeries::TimeDiscretization const time_discretization{nr_steps};

    // Space points
    std::size_t const nr_points = 5;
    std::string const crs = "blah";

    hl::TimeSeries::SpaceDomain::Coordinates coordinates(2 * nr_points);

    std::for_each(
        coordinates.begin(), coordinates.end(),
        [](hl::TimeSeries::SpaceDomain::Coordinate& coordinate) {
            coordinate = std::rand();
        });

    hl::TimeSeries::SpaceDomain const space_domain{
        crs, hl::TimeSeries::SpaceDomain::Coordinates{coordinates}};

    std::string const phenomenon_name = "my_phenomenon";
    std::string const property_set_name = "my_property_set";
    std::string const property_name = "my_property";

    // Create time series without values and query its contents
    auto dataset = lue::create_dataset(dataset_name);
    auto time_series = hl::create_time_series(
        dataset, phenomenon_name, property_set_name,
        time_domain, space_domain, property_name, time_discretization);

    BOOST_CHECK_EQUAL(time_series.name(), property_name);
    BOOST_CHECK_EQUAL(time_series.nr_items(), nr_points);

    // Time domain
    auto const& time_domain_read = time_series.time_domain();
    auto const& clock_read = time_domain_read.clock();
    BOOST_CHECK(clock_read.unit() == time::Unit::hour);
    auto const& time_coordinates_read = time_domain_read.coordinates();
    BOOST_CHECK_EQUAL(time_coordinates_read.size(), 2);
    BOOST_CHECK_EQUAL(time_coordinates_read[0], start_time_point);
    BOOST_CHECK_EQUAL(time_coordinates_read[1], end_time_point);

    // Space domain
    auto const& space_domain_read = time_series.space_domain();
    BOOST_CHECK_EQUAL(space_domain_read.crs(), crs);
    BOOST_CHECK_EQUAL_COLLECTIONS(
            space_domain_read.coordinates().begin(),
            space_domain_read.coordinates().end(),
            coordinates.begin(), coordinates.end());

    // Time discretization
    auto const& time_discretization_read = time_series.time_discretization();
    BOOST_CHECK_EQUAL(time_discretization_read.nr_steps(), nr_steps);

    // Write values to time series and test the result of reading them
    // back in
    // For each point an array with values. Each array must be written
    // individually. The item index make sure that the values end up in
    // the correct position in the dataset.

    std::vector<std::vector<double>> values_written(nr_points);

    for(std::size_t i = 0; i < nr_points; ++i) {
        values_written[i].resize(nr_steps);
        std::iota(values_written[i].begin(), values_written[i].end(), i);
        time_series.write(i, values_written[i].data());
    }

    for(size_t i = 0; i < nr_points; ++i) {
        std::vector<double> values_read(nr_steps);
        time_series.read(i, values_read.data());
        BOOST_CHECK_EQUAL_COLLECTIONS(
            values_read.begin(), values_read.end(),
            values_written[i].begin(), values_written[i].end());
    }
}
