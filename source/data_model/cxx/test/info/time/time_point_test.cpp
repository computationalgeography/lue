#define BOOST_TEST_MODULE lue info time time_point
#include <boost/test/unit_test.hpp>
#include "lue/info/time/time_point.hpp"
#include "lue/core/tag.hpp"
#include "lue/test.hpp"


class Fixture:
    public lue::test::FileFixture
{

public:

    Fixture()
        : FileFixture{"value.h5"},
          _filename{"value.h5"},
          _file{std::make_unique<lue::hdf5::File>(
            lue::hdf5::create_file(_filename))},
          _value{std::make_unique<lue::TimePoint>(
            lue::create_time_point(*_file))}
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
    std::unique_ptr<lue::TimePoint> _value;

};


BOOST_FIXTURE_TEST_CASE(create, Fixture)
{
    using namespace lue;

    auto const& value = this->value();

    BOOST_CHECK_EQUAL(value.id().name(), coordinates_tag);
    BOOST_CHECK(
        value.memory_datatype() ==
        hdf5::Datatype{
            hdf5::NativeDatatypeTraits<time::DurationCount>::type_id()});
    BOOST_CHECK_EQUAL(value.nr_arrays(), 0);
    BOOST_CHECK(value.array_shape() == hdf5::Shape{1});
}


// Most functionality is inherited and tested elsewhere.
