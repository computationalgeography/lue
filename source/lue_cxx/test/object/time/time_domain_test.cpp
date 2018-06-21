#define BOOST_TEST_MODULE lue object time time_domain
#include <boost/test/unit_test.hpp>
#include "lue/object/time/time_domain.hpp"
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
        _domain{std::make_unique<lue::TimeDomain>(
            lue::create_time_domain(*_file, _configuration))}
    {
    }

    ~Fixture()
    {
    }

    auto& configuration()
    {
        return _configuration;
    }

    auto& domain()
    {
        return *_domain;
    }

private:

    std::string const _filename;
    std::unique_ptr<lue::hdf5::File> _file;
    lue::TimeConfiguration const _configuration;
    std::unique_ptr<lue::TimeDomain> _domain;

};


BOOST_FIXTURE_TEST_CASE(create, Fixture)
{
    auto const& domain = this->domain();

    BOOST_CHECK(domain.configuration() == configuration());
}
