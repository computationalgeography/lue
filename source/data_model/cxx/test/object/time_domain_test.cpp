#define BOOST_TEST_MODULE lue object time_domain
#include "lue/object/time_domain.hpp"
#include "lue/test.hpp"
#include <boost/test/included/unit_test.hpp>


class Fixture: public lue::data_model::test::FileFixture
{

    public:

        Fixture():
            FileFixture{"property.h5"},
            _filename{"property.h5"},
            _file{std::make_unique<lue::hdf5::File>(lue::hdf5::create_file(_filename))},
            _configuration{lue::data_model::TimeDomainItemType::box},
            _clock{lue::data_model::time::Unit::day, 1},
            _domain{std::make_unique<lue::data_model::TimeDomain>(
                lue::data_model::create_time_domain(*_file, _configuration, _clock))}
        {
        }

        Fixture(Fixture const&) = delete;

        Fixture(Fixture&&) = delete;

        ~Fixture() override = default;

        Fixture& operator=(Fixture const&) = delete;

        Fixture& operator=(Fixture&&) = delete;

        auto const& configuration() const
        {
            return _configuration;
        }

        auto const& clock() const
        {
            return _clock;
        }

        auto& domain()
        {
            return *_domain;
        }

    private:

        std::string const _filename;
        std::unique_ptr<lue::hdf5::File> _file;
        lue::data_model::TimeConfiguration const _configuration;
        lue::data_model::Clock const _clock;
        std::unique_ptr<lue::data_model::TimeDomain> _domain;
};


BOOST_FIXTURE_TEST_CASE(create, Fixture)
{
    auto const& domain = this->domain();

    BOOST_CHECK_EQUAL(domain.configuration(), configuration());
    BOOST_CHECK_EQUAL(domain.clock(), clock());
}
