#define BOOST_TEST_MODULE lue array same_shape variable_shape value
#include "lue/array/same_shape/variable_shape/value.hpp"
#include "lue/hdf5/test/file_fixture.hpp"
#include <boost/test/included/unit_test.hpp>


class Fixture: public lue::hdf5::FileFixture
{

    public:

        Fixture():
            FileFixture{"value.h5"},
            _filename{"value.h5"},
            _value_name{"my_value"},
            _datatype{lue::hdf5::NativeDatatypeTraits<int32_t>::type_id()},
            _rank{2},
            _file{std::make_unique<lue::hdf5::File>(lue::hdf5::create_file(_filename))},
            _value{std::make_unique<lue::data_model::same_shape::variable_shape::Value>(
                lue::data_model::same_shape::variable_shape::create_value(
                    *_file, _value_name, _datatype, _rank))}
        {
        }

        Fixture(Fixture const&) = delete;

        Fixture(Fixture&&) = delete;

        ~Fixture() override = default;

        Fixture& operator=(Fixture const&) = delete;

        Fixture& operator=(Fixture&&) = delete;

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
        std::unique_ptr<lue::data_model::same_shape::variable_shape::Value> _value;
};


BOOST_FIXTURE_TEST_CASE(create_value, Fixture)
{
    auto const& value = this->value();

    BOOST_CHECK(value.memory_datatype() == datatype());
    BOOST_CHECK(value.file_datatype() == lue::hdf5::file_datatype(datatype()));
    BOOST_CHECK_EQUAL(value.rank(), rank());
    BOOST_CHECK_EQUAL(value.nr_locations_in_time(), 0);
}


BOOST_FIXTURE_TEST_CASE(update_all_object_arrays_per_location_in_time, Fixture)
{
    auto& value = this->value();

    lue::data_model::Count const nr_locations_in_time1 = 3;
    lue::data_model::Counts const nr_active_objects1{3, 4, 2};
    lue::data_model::Shapes const array_shapes1{{3, 2}, {5, 4}, {7, 6}};

    {
        for (std::size_t t = 0; t < nr_locations_in_time1; ++t)
        {
            auto t_value = value.expand(t, nr_active_objects1[t], array_shapes1[t]);

            BOOST_CHECK_EQUAL(t_value.nr_arrays(), nr_active_objects1[t]);
            BOOST_CHECK(t_value.array_shape() == array_shapes1[t]);
            BOOST_CHECK(t_value.memory_datatype() == datatype());
            BOOST_CHECK(t_value.file_datatype() == lue::hdf5::file_datatype(datatype()));
        }

        BOOST_REQUIRE_EQUAL(value.nr_locations_in_time(), nr_locations_in_time1);

        for (std::size_t t = 0; t < nr_locations_in_time1; ++t)
        {
            auto t_value = value[t];

            BOOST_CHECK_EQUAL(t_value.nr_arrays(), nr_active_objects1[t]);
            BOOST_CHECK(t_value.array_shape() == array_shapes1[t]);
            BOOST_CHECK(t_value.memory_datatype() == datatype());
            BOOST_CHECK(t_value.file_datatype() == lue::hdf5::file_datatype(datatype()));
        }
    }

    lue::data_model::Count const nr_locations_in_time2 = 3;
    lue::data_model::Counts const nr_active_objects2{8, 1, 3};
    lue::data_model::Shapes const array_shapes2{{2, 3}, {4, 5}, {6, 7}};

    {
        for (std::size_t t = 0; t < nr_locations_in_time2; ++t)
        {
            auto t_value = value.expand(t + nr_locations_in_time1, nr_active_objects2[t], array_shapes2[t]);

            BOOST_CHECK_EQUAL(t_value.nr_arrays(), nr_active_objects2[t]);
            BOOST_CHECK(t_value.array_shape() == array_shapes2[t]);
            BOOST_CHECK(t_value.memory_datatype() == datatype());
            BOOST_CHECK(t_value.file_datatype() == lue::hdf5::file_datatype(datatype()));
        }

        BOOST_REQUIRE_EQUAL(value.nr_locations_in_time(), nr_locations_in_time1 + nr_locations_in_time2);

        for (std::size_t t = 0; t < nr_locations_in_time2; ++t)
        {
            auto t_value = value[t + nr_locations_in_time1];

            BOOST_CHECK_EQUAL(t_value.nr_arrays(), nr_active_objects2[t]);
            BOOST_CHECK(t_value.array_shape() == array_shapes2[t]);
            BOOST_CHECK(t_value.memory_datatype() == datatype());
            BOOST_CHECK(t_value.file_datatype() == lue::hdf5::file_datatype(datatype()));
        }
    }


    // Reading and writing elements is handled by the same_shape::Value
    // instance returned by operator[]. This functionality is already
    // tested elsewhere.
}
