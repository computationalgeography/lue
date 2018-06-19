#define BOOST_TEST_MODULE lue info property same_shape property
#include <boost/test/unit_test.hpp>
#include "lue/info/property/same_shape/property.hpp"
#include "lue/test.hpp"


class Fixture:
    public lue::test::FileFixture
{

public:

    Fixture():
        FileFixture{"property.h5"},
        _filename{"property.h5"},
        _name{"my_property"},
        _datatype{lue::hdf5::NativeDatatypeTraits<int32_t>::type_id()},
        _array_shape{3, 2},
        _file{std::make_unique<lue::hdf5::File>(
            lue::hdf5::create_file(_filename))},
        _property{std::make_unique<lue::same_shape::Property>(
            lue::same_shape::create_property(
                *_file, _name, _datatype, _array_shape))}
    {
    }

    ~Fixture()
    {
    }

    auto& property()
    {
        return *_property;
    }

    auto const& name() const
    {
        return _name;
    }

    auto const& datatype() const
    {
        return _datatype;
    }

    auto const& array_shape() const
    {
        return _array_shape;
    }

private:

    std::string const _filename;
    std::string const _name;
    lue::hdf5::Datatype const _datatype;
    lue::hdf5::Shape const _array_shape;
    std::unique_ptr<lue::hdf5::File> _file;
    std::unique_ptr<lue::same_shape::Property> _property;

};


BOOST_FIXTURE_TEST_CASE(create_property, Fixture)
{
    auto const& property = this->property();

    BOOST_CHECK_EQUAL(property.name(), name());
}
