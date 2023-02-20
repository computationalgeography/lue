#define BOOST_TEST_MODULE lue object space_domain
#include "lue/object/space_domain.hpp"
#include "lue/test.hpp"
#include <boost/test/unit_test.hpp>


class Fixture: public lue::data_model::test::FileFixture
{

    public:

        Fixture():
            FileFixture{"property.h5"},
            _filename{"property.h5"},
            _file{std::make_unique<lue::hdf5::File>(lue::hdf5::create_file(_filename))},
            _configuration{lue::data_model::Mobility::stationary, lue::data_model::SpaceDomainItemType::box},
            _datatype{lue::hdf5::NativeDatatypeTraits<double>::type_id()},
            _rank{2},
            _domain{std::make_unique<lue::data_model::SpaceDomain>(
                lue::data_model::create_space_domain(*_file, _configuration, _datatype, _rank))}
        {
        }

        Fixture(Fixture const&) = delete;

        Fixture(Fixture&&) = delete;

        ~Fixture() override = default;

        Fixture& operator=(Fixture const&) = delete;

        Fixture& operator=(Fixture&&) = delete;

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
        lue::data_model::SpaceConfiguration const _configuration;
        lue::hdf5::Datatype _datatype;
        std::size_t const _rank;
        std::unique_ptr<lue::data_model::SpaceDomain> _domain;
};


BOOST_FIXTURE_TEST_CASE(create, Fixture)
{
    auto const& domain = this->domain();

    BOOST_CHECK(domain.configuration() == configuration());
}
