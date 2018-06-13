#define BOOST_TEST_MODULE lue array same_shape variable_shape value
#include <boost/test/unit_test.hpp>
#include "lue/array/same_shape/variable_shape/value.hpp"
#include "lue/test.hpp"


class Fixture:
    public lue::test::FileFixture
{

public:

    Fixture()
        : FileFixture{"value.h5"},
          _filename{"value.h5"},
          _value_name{"my_value"},
          _datatype{lue::hdf5::NativeDatatypeTraits<int32_t>::type_id()},
          // _nr_locations_in_time{3},
          _rank{2},
          // _array_shape{3, 2, 5, 4, 7, 6},
          _file{std::make_unique<lue::hdf5::File>(
            lue::hdf5::create_file(_filename))},
          _value{std::make_unique<lue::same_shape::variable_shape::Value>(
            lue::same_shape::variable_shape::create_value(
                *_file, _value_name, _datatype, _rank))}
    {
    }

    ~Fixture()
    {
    }

    auto& value()
    {
        return *_value;
    }

    auto rank() const
    {
        return _rank;
    }

    auto const& datatype() const
    {
        return _datatype;
    }

private:

    std::string const _filename;
    std::string const _value_name;
    lue::hdf5::Datatype const _datatype;
    // hsize_t const _nr_locations_in_time;
    lue::Rank const _rank;
    // lue::hdf5::Shape const _array_shape;
    std::unique_ptr<lue::hdf5::File> _file;
    std::unique_ptr<lue::same_shape::variable_shape::Value> _value;

};


BOOST_FIXTURE_TEST_CASE(create_value, Fixture)
{
    auto const& value = this->value();

    BOOST_CHECK_EQUAL(value.rank(), rank());
    BOOST_CHECK_EQUAL(value.nr_locations_in_time(), 0);

    BOOST_CHECK(value.memory_datatype() == datatype());
    BOOST_CHECK(
        value.file_datatype() == lue::hdf5::file_datatype(datatype()));
}


BOOST_FIXTURE_TEST_CASE(update_all_object_arrays_per_location_in_time, Fixture)
{
    auto& value = this->value();

    lue::Count const nr_locations_in_time = 3;
    lue::Counts const nr_active_objects{3, 4, 2};
    lue::Shapes const array_shapes{{3, 2}, {5, 4}, {7, 6}};

    for(std::size_t t = 0; t < nr_locations_in_time; ++t) {
        auto t_value = value.reserve(
            t, nr_active_objects[t], array_shapes[t]);

        BOOST_CHECK_EQUAL(t_value.nr_arrays(), nr_active_objects[t]);
        BOOST_CHECK(t_value.array_shape() == array_shapes[t]);
        BOOST_CHECK(t_value.memory_datatype() == datatype());
        BOOST_CHECK(
            t_value.file_datatype() == lue::hdf5::file_datatype(datatype()));
    }

    BOOST_REQUIRE_EQUAL(value.nr_locations_in_time(), nr_locations_in_time);

    for(std::size_t t = 0; t < nr_locations_in_time; ++t) {
        auto t_value = value[t];

        BOOST_CHECK_EQUAL(t_value.nr_arrays(), nr_active_objects[t]);
        BOOST_CHECK(t_value.array_shape() == array_shapes[t]);
        BOOST_CHECK(t_value.memory_datatype() == datatype());
        BOOST_CHECK(
            t_value.file_datatype() == lue::hdf5::file_datatype(datatype()));
    }

    // Reading and writing elements is handled by the same_shape::Value
    // instance returned by operator[]. This functionality is already
    // tested elsewhere.
}
