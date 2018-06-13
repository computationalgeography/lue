#define BOOST_TEST_MODULE lue array same_shape value
#include <boost/test/unit_test.hpp>
#include "lue/array/same_shape/value.hpp"
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
          _nr_rows{3},
          _nr_cols{2},
          _array_shape{_nr_rows, _nr_cols},
          _file{std::make_unique<lue::hdf5::File>(
            lue::hdf5::create_file(_filename))},
          _value{std::make_unique<lue::same_shape::Value>(
            lue::same_shape::create_value(
                *_file, _value_name, _datatype, _array_shape))}
    {
    }

    ~Fixture()
    {
    }

    auto& value()
    {
        return *_value;
    }

    auto const& array_shape() const
    {
        return _array_shape;
    }

    auto const& datatype() const
    {
        return _datatype;
    }

    auto nr_cells() const
    {
        return _nr_rows * _nr_cols;
    }

private:

    std::string const _filename;
    std::string const _value_name;
    lue::hdf5::Datatype const _datatype;
    std::size_t const _nr_rows;
    std::size_t const _nr_cols;
    lue::hdf5::Shape const _array_shape;
    std::unique_ptr<lue::hdf5::File> _file;
    std::unique_ptr<lue::same_shape::Value> _value;

};


BOOST_FIXTURE_TEST_CASE(create_value, Fixture)
{
    auto const& value = this->value();

    BOOST_CHECK(value.memory_datatype() == datatype());
    BOOST_CHECK(
        value.file_datatype() == lue::hdf5::file_datatype(datatype()));
    BOOST_CHECK_EQUAL(value.nr_arrays(), 0);
    BOOST_CHECK(value.array_shape() == array_shape());
}


BOOST_FIXTURE_TEST_CASE(update_all_object_arrays, Fixture)
{
    auto& value = this->value();

    lue::Count const nr_arrays = 3;
    value.reserve(nr_arrays);

    BOOST_CHECK_EQUAL(value.nr_arrays(), nr_arrays);

    // Write and read arrays for all objects
    std::vector<int32_t> elements = {
        // First item, 6 elements
        11, 12,
        13, 14,
        15, 16,

        // Second item, 6 elements
        21, 22,
        23, 24,
        25, 26,

        31, 32,
        33, 34,
        35, 36,
    };
    value.write(elements.data());

    std::vector<int32_t> elements_read(nr_arrays * nr_cells());
    value.read(elements_read.data());

    BOOST_CHECK_EQUAL_COLLECTIONS(
        elements_read.begin(), elements_read.end(),
        elements.begin(), elements.end());
}


BOOST_FIXTURE_TEST_CASE(update_range_of_object_arrays, Fixture)
{
    auto& value = this->value();

    lue::Count const nr_arrays = 3;
    value.reserve(nr_arrays);

    BOOST_CHECK_EQUAL(value.nr_arrays(), nr_arrays);

    // Write and read arrays for a range of objects
    std::vector<int32_t> elements = {
        21, 22,
        23, 24,
        25, 26,

        31, 32,
        33, 34,
        35, 36,
    };
    value.write({1, 3}, elements.data());

    std::vector<int32_t> elements_read(2 * nr_cells());
    value.read({1, 3}, elements_read.data());

    BOOST_CHECK_EQUAL_COLLECTIONS(
        elements_read.begin(), elements_read.end(),
        elements.begin(), elements.end());
}


BOOST_FIXTURE_TEST_CASE(update_individual_object_arrays, Fixture)
{
    auto& value = this->value();

    lue::Count const nr_arrays = 3;
    value.reserve(nr_arrays);

    BOOST_CHECK_EQUAL(value.nr_arrays(), nr_arrays);

    // Write and read array for item with idx 2
    std::vector<int32_t> elements = {
        51, 52,
        53, 54,
        55, 56,
    };
    value.write(2, elements.data());

    std::vector<int32_t> elements_read(nr_cells());
    value.read(2, elements_read.data());

    BOOST_CHECK_EQUAL_COLLECTIONS(
        elements_read.begin(), elements_read.end(),
        elements.begin(), elements.end());
}
