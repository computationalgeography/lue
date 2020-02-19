#define BOOST_TEST_MODULE lue info time time_cell
#include <boost/test/unit_test.hpp>
#include "lue/info/time/time_cell.hpp"
#include "lue/core/tag.hpp"
#include "lue/test.hpp"
#include <iostream>


class Fixture:
    public lue::data_model::test::FileFixture
{

public:

    Fixture()
        : FileFixture{"value.h5"},
          _filename{"value.h5"},
          _file{std::make_unique<lue::hdf5::File>(
            lue::hdf5::create_file(_filename))},
          _value{std::make_unique<lue::data_model::TimeCell>(
            lue::data_model::create_time_cell(*_file))}
    {
    }

    ~Fixture()
    {
    }

    auto& value()
    {
        return *_value;
    }

private:

    std::string const _filename;
    std::unique_ptr<lue::hdf5::File> _file;
    std::unique_ptr<lue::data_model::TimeCell> _value;

};


BOOST_FIXTURE_TEST_CASE(create, Fixture)
{
    using namespace lue;

    auto const& value = this->value();

    BOOST_CHECK_EQUAL(value.id().name(), data_model::coordinates_tag);
    BOOST_CHECK(
        value.memory_datatype() ==
        hdf5::Datatype{
            hdf5::NativeDatatypeTraits<data_model::time::DurationCount>::type_id()});
    BOOST_CHECK_EQUAL(value.nr_arrays(), 0);
    BOOST_CHECK(value.array_shape() == hdf5::Shape{2});

    auto const& count = value.count();

    BOOST_CHECK_EQUAL(count.id().name(), data_model::time_discretization_tag);
    BOOST_CHECK(
        count.memory_datatype() ==
        hdf5::Datatype{hdf5::NativeDatatypeTraits<data_model::Count>::type_id()});
    BOOST_CHECK_EQUAL(count.nr_arrays(), 0);
    BOOST_CHECK(count.array_shape() == hdf5::Shape{});
}


// Most functionality is inherited and tested elsewhere.
