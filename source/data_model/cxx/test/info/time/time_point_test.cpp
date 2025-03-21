#define BOOST_TEST_MODULE lue info time time_point
#include "lue/core/tag.hpp"
#include "lue/hdf5/test/file_fixture.hpp"
#include "lue/info/time/time_point.hpp"
#include <boost/test/included/unit_test.hpp>


class Fixture: public lue::hdf5::FileFixture
{

    public:

        Fixture():
            FileFixture{"value.h5"},
            _filename{"value.h5"},
            _file{std::make_unique<lue::hdf5::File>(lue::hdf5::create_file(_filename))},
            _value{std::make_unique<lue::data_model::TimePoint>(lue::data_model::create_time_point(*_file))}
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

    private:

        std::string const _filename;
        std::unique_ptr<lue::hdf5::File> _file;
        std::unique_ptr<lue::data_model::TimePoint> _value;
};


BOOST_FIXTURE_TEST_CASE(create, Fixture)
{
    namespace lh5 = lue::hdf5;
    namespace ldm = lue::data_model;

    auto const& value = this->value();

    BOOST_CHECK_EQUAL(value.id().name(), ldm::coordinates_tag);
    BOOST_CHECK(
        value.memory_datatype() ==
        lh5::Datatype{lh5::NativeDatatypeTraits<ldm::time::DurationCount>::type_id()});
    BOOST_CHECK_EQUAL(value.nr_arrays(), 0);
    BOOST_CHECK(value.array_shape() == lh5::Shape{1});
}


// Most functionality is inherited and tested elsewhere.
