#define BOOST_TEST_MODULE lue array different_shape value
#include <boost/test/unit_test.hpp>
#include "lue/array/different_shape/value.hpp"
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
          _rank{2},
          _file{std::make_unique<lue::hdf5::File>(
            lue::hdf5::create_file(_filename))},
          _value{std::make_unique<lue::different_shape::Value>(
            lue::different_shape::create_value(
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
    lue::Rank const _rank;
    std::unique_ptr<lue::hdf5::File> _file;
    std::unique_ptr<lue::different_shape::Value> _value;

};


BOOST_FIXTURE_TEST_CASE(create_value, Fixture)
{
    auto const& value = this->value();

    BOOST_CHECK(value.memory_datatype() == datatype());
    BOOST_CHECK(
        value.file_datatype() == lue::hdf5::file_datatype(datatype()));
    BOOST_CHECK_EQUAL(value.rank(), rank());
    BOOST_CHECK_EQUAL(value.nr_objects(), 0);
}


BOOST_FIXTURE_TEST_CASE(update_all_object_arrays, Fixture)
{
    auto& value = this->value();

    lue::IDs const ids1{5, 7, 9};
    lue::Shapes const array_shapes1{{3, 2}, {5, 4}, {7, 6}};

    {
        value.expand(ids1.size(), ids1.data(), array_shapes1.data());

        BOOST_REQUIRE_EQUAL(value.nr_objects(), ids1.size());

        for(std::size_t o = 0; o < ids1.size(); ++o) {
            auto array = value[ids1[o]];

            BOOST_CHECK(array.shape() == array_shapes1[o]);
        }
    }

    lue::IDs const ids2{6, 8, 1};
    lue::Shapes const array_shapes2{{2, 3}, {4, 5}, {6, 7}};

    {
        value.expand(ids2.size(), ids2.data(), array_shapes2.data());

        BOOST_REQUIRE_EQUAL(value.nr_objects(), ids1.size() + ids2.size());

        for(std::size_t o = 0; o < ids2.size(); ++o) {
            auto array = value[ids2[o]];

            BOOST_CHECK(array.shape() == array_shapes2[o]);
        }
    }


    // Reading and writing elements is handled by the Array
    // instance returned by operator[]. This functionality is already
    // tested elsewhere.
}
