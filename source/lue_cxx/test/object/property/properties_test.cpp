#define BOOST_TEST_MODULE lue object property properties
#include <boost/test/unit_test.hpp>
#include "lue/object/property/properties.hpp"
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
        _properties{std::make_unique<lue::Properties>(
            lue::create_properties(*_file))}
    {
    }

    ~Fixture()
    {
    }

    auto& properties()
    {
        return *_properties;
    }

private:

    std::string const _filename;
    std::unique_ptr<lue::hdf5::File> _file;
    std::unique_ptr<lue::Properties> _properties;

};


BOOST_FIXTURE_TEST_CASE(create, Fixture)
{
    auto const& properties = this->properties();

    // same_shape
    {
        using Properties = lue::same_shape::Properties;
        auto const& collection = properties.collection<Properties>();
        BOOST_CHECK_EQUAL(collection.size(), 0);

        // ...
    }

    // different_shape
    {
        using Properties = lue::different_shape::Properties;
        auto const& collection = properties.collection<Properties>();
        BOOST_CHECK_EQUAL(collection.size(), 0);

        // ...
    }

    // same_shape x constant_shape
    {
        using Properties = lue::same_shape::constant_shape::Properties;
        auto const& collection = properties.collection<Properties>();
        BOOST_CHECK_EQUAL(collection.size(), 0);

        // ...
    }

    // ...
}
