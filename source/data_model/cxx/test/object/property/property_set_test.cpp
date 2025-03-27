#define BOOST_TEST_MODULE lue object property property_set
#include "lue/hdf5/test/file_fixture.hpp"
#include "lue/object/property/property_set.hpp"
#include <boost/test/included/unit_test.hpp>


class Fixture: public lue::hdf5::FileFixture
{

    public:

        Fixture():
            FileFixture{"property.h5"},
            _filename{"property.h5"},
            _file{std::make_unique<lue::hdf5::File>(lue::hdf5::create_file(_filename))},
            _property_set_name{"my_property_set"},
            _time_configuration{lue::data_model::TimeDomainItemType::box},
            _clock{lue::data_model::time::Unit::day, 1},
            _space_configuration{
                lue::data_model::Mobility::stationary, lue::data_model::SpaceDomainItemType::point},
            _space_coordinate_datatype{lue::hdf5::NativeDatatypeTraits<double>::type_id()},
            _space_rank{2},
            _property_set{std::make_unique<lue::data_model::PropertySet>(lue::data_model::create_property_set(
                *_file,
                _property_set_name,
                _time_configuration,
                _clock,
                _space_configuration,
                _space_coordinate_datatype,
                _space_rank))}
        {
        }

        Fixture(Fixture const&) = delete;

        Fixture(Fixture&&) = delete;

        ~Fixture() override = default;

        Fixture& operator=(Fixture const&) = delete;

        Fixture& operator=(Fixture&&) = delete;

        auto const& property_set_name() const
        {
            return _property_set_name;
        }

        auto const& time_configuration() const
        {
            return _time_configuration;
        }

        auto const& space_configuration() const
        {
            return _space_configuration;
        }

        auto& property_set()
        {
            return *_property_set;
        }

    private:

        std::string const _filename;
        std::unique_ptr<lue::hdf5::File> _file;
        std::string const _property_set_name;
        lue::data_model::TimeConfiguration _time_configuration;
        lue::data_model::Clock const _clock;
        lue::data_model::SpaceConfiguration _space_configuration;
        lue::hdf5::Datatype const _space_coordinate_datatype;
        std::size_t const _space_rank;
        std::unique_ptr<lue::data_model::PropertySet> _property_set;
};


BOOST_FIXTURE_TEST_CASE(create, Fixture)
{
    auto const& property_set = this->property_set();
    auto const& time_domain = property_set.time_domain();
    auto const& space_domain = property_set.space_domain();
    auto const& properties = property_set.properties();

    BOOST_CHECK_EQUAL(property_set.id().name(), property_set_name());
    BOOST_CHECK(time_domain.configuration() == time_configuration());
    BOOST_CHECK(space_domain.configuration() == space_configuration());
    // TODO(KDJ) Grab space domain and check it
    // ...

    BOOST_CHECK(properties.empty());

    // same_shape
    {
        using Properties = lue::data_model::same_shape::Properties;
        auto const& collection = properties.collection<Properties>();
        BOOST_CHECK_EQUAL(collection.size(), 0);

        // ...
    }

    // same_shape x constant_shape
    {
        using Properties = lue::data_model::same_shape::constant_shape::Properties;
        auto const& collection = properties.collection<Properties>();
        BOOST_CHECK_EQUAL(collection.size(), 0);

        // ...
    }

    // same_shape x variable_shape
    {
        using Properties = lue::data_model::same_shape::variable_shape::Properties;
        auto const& collection = properties.collection<Properties>();
        BOOST_CHECK_EQUAL(collection.size(), 0);

        // ...
    }

    // different_shape
    {
        using Properties = lue::data_model::different_shape::Properties;
        auto const& collection = properties.collection<Properties>();
        BOOST_CHECK_EQUAL(collection.size(), 0);

        // ...
    }

    // different_shape x constant_shape
    {
        using Properties = lue::data_model::different_shape::constant_shape::Properties;
        auto const& collection = properties.collection<Properties>();
        BOOST_CHECK_EQUAL(collection.size(), 0);

        // ...
    }

    // different_shape x variable_shape
    {
        using Properties = lue::data_model::different_shape::constant_shape::Properties;
        auto const& collection = properties.collection<Properties>();
        BOOST_CHECK_EQUAL(collection.size(), 0);

        // ...
    }

    // ...
}
