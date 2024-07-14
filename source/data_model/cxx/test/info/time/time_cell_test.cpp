#define BOOST_TEST_MODULE lue info time time_cell
#include "lue/core/tag.hpp"
#include "lue/info/time/time_cell.hpp"
#include "lue/test.hpp"
#include <boost/test/included/unit_test.hpp>
#include <iostream>


class Fixture: public lue::data_model::test::FileFixture
{

    public:

        Fixture():
            FileFixture{"value.h5"},
            _filename{"value.h5"},
            _file{std::make_unique<lue::hdf5::File>(lue::hdf5::create_file(_filename))},
            _value{std::make_unique<lue::data_model::TimeCell>(lue::data_model::create_time_cell(*_file))}
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
        std::unique_ptr<lue::data_model::TimeCell> _value;
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
    BOOST_CHECK(value.array_shape() == lh5::Shape{2});

    auto const& count = value.count();

    BOOST_CHECK_EQUAL(count.id().name(), ldm::time_discretization_tag);
    BOOST_CHECK(count.memory_datatype() == lh5::Datatype{lh5::NativeDatatypeTraits<ldm::Count>::type_id()});
    BOOST_CHECK_EQUAL(count.nr_arrays(), 0);
    BOOST_CHECK(count.array_shape().empty());
}


// Most functionality is inherited and tested elsewhere.
