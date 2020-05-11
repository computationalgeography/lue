#define BOOST_TEST_MODULE lue array different_shape variable_shape value
#include <boost/test/unit_test.hpp>
#include "lue/array/different_shape/variable_shape/value.hpp"
#include "lue/test.hpp"


class Fixture:
    public lue::data_model::test::FileFixture
{

public:

    Fixture()
        : FileFixture{"value.h5"},
          _filename{"value.h5"},
          _value_name{"my_value"},
          _datatype{lue::hdf5::NativeDatatypeTraits<int32_t>::type_id()},
          _rank{2},
          _file{std::make_unique<lue::hdf5::File>(
            lue::hdf5::create_file(_filename))},
          _value{std::make_unique<lue::data_model::different_shape::variable_shape::Value>(
            lue::data_model::different_shape::variable_shape::create_value(
                *_file, _value_name, _datatype, _rank))}
    {
    }

    Fixture(Fixture const&)=delete;

    Fixture(Fixture&&)=delete;

    ~Fixture() override =default;

    Fixture& operator=(Fixture const&)=delete;

    Fixture& operator=(Fixture&&)=delete;

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
    lue::data_model::Rank const _rank;
    std::unique_ptr<lue::hdf5::File> _file;
    std::unique_ptr<lue::data_model::different_shape::variable_shape::Value>
        _value;

};


BOOST_FIXTURE_TEST_CASE(create_value, Fixture)
{
    auto const& value = this->value();

    BOOST_CHECK(value.memory_datatype() == datatype());
    BOOST_CHECK(
        value.file_datatype() == lue::hdf5::file_datatype(datatype()));
    BOOST_CHECK_EQUAL(value.rank(), rank());
    BOOST_CHECK_EQUAL(value.nr_locations_in_time(), 0);
}


BOOST_FIXTURE_TEST_CASE(update_all_object_arrays_per_location_in_time, Fixture)
{
    auto& value = this->value();

    lue::data_model::Count const nr_locations_in_time = 3;
    lue::data_model::Counts const nr_active_objects{3, 4, 2};
    std::vector<lue::data_model::IDs> const active_ids{
        {5, 6, 7},  // 3 active objects
        {5, 6, 8, 9},  // 4
        {8, 3}  // 2
    };
    std::vector<lue::data_model::Shapes> const array_shapes{
        {{3, 2}, {5, 4}, {7, 6}},  // 3 active objects
        {{5, 4}, {7, 6}, {3, 2}, {6, 5}},  // 4
        {{7, 6}, {3, 2}}  // 2
    };

    for(std::size_t t = 0; t < nr_locations_in_time; ++t) {
        auto t_value = value.expand(
            t, nr_active_objects[t], active_ids[t].data(),
            array_shapes[t].data());

        BOOST_REQUIRE_EQUAL(t_value.nr_objects(), nr_active_objects[t]);

        for(std::size_t o = 0; o < t_value.nr_objects(); ++o) {
            auto array = t_value[active_ids[t][o]];

            BOOST_CHECK(array.memory_datatype() == datatype());
            BOOST_CHECK(
                array.file_datatype() == lue::hdf5::file_datatype(datatype()));
            BOOST_CHECK(array.shape() == array_shapes[t][o]);
        }
    }

    BOOST_REQUIRE_EQUAL(value.nr_locations_in_time(), nr_locations_in_time);

    for(std::size_t t = 0; t < nr_locations_in_time; ++t) {
        auto t_value = value[t];

        BOOST_REQUIRE_EQUAL(t_value.nr_objects(), nr_active_objects[t]);

        for(std::size_t o = 0; o < t_value.nr_objects(); ++o) {
            auto array = t_value[active_ids[t][o]];

            BOOST_CHECK(array.memory_datatype() == datatype());
            BOOST_CHECK(
                array.file_datatype() == lue::hdf5::file_datatype(datatype()));
            BOOST_CHECK(array.shape() == array_shapes[t][o]);
        }
    }

    // Reading and writing elements is handled by the different_shape::Value
    // instance returned by operator[]. This functionality is already
    // tested elsewhere.
}
