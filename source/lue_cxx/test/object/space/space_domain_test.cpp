#define BOOST_TEST_MODULE lue object space space_domain
#include <boost/test/unit_test.hpp>
#include "lue/object/space/space_domain.hpp"
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
            lue::Mobility::stationary,
            lue::SpaceDomainItemType::box},
        _domain{std::make_unique<lue::SpaceDomain>(
            lue::create_space_domain(*_file, _configuration))}
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
    lue::SpaceConfiguration const _configuration;
    std::unique_ptr<lue::SpaceDomain> _domain;

};


BOOST_FIXTURE_TEST_CASE(create_property, Fixture)
{
    auto const& domain = this->domain();

    BOOST_CHECK(domain.configuration() == configuration());
}
