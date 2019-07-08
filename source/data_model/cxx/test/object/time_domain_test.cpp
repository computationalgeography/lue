#define BOOST_TEST_MODULE lue object time_domain
#include <boost/test/unit_test.hpp>
#include "lue/object/time_domain.hpp"
#include "lue/test.hpp"


class Fixture:
    public lue::test::FileFixture
{

public:

    Fixture():
        FileFixture{"property.h5"},
        _filename{"property.h5"},
        _file{std::make_unique<lue::hdf5::File>(
            lue::hdf5::create_file(_filename))},
        _configuration{
            lue::TimeDomainItemType::box},
        _clock{lue::time::Unit::day, 1},
        _domain{std::make_unique<lue::TimeDomain>(
            lue::create_time_domain(*_file, _configuration, _clock))}
    {
    }

    ~Fixture()
    {
    }

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
    lue::TimeConfiguration const _configuration;
    lue::Clock const _clock;
    std::unique_ptr<lue::TimeDomain> _domain;

};


BOOST_FIXTURE_TEST_CASE(create, Fixture)
{
    auto const& domain = this->domain();

    BOOST_CHECK_EQUAL(domain.configuration(), configuration());
    BOOST_CHECK_EQUAL(domain.clock(), clock());
}
