#define BOOST_TEST_MODULE lue info property same_shape variable_shape property
#include <boost/test/unit_test.hpp>
#include "lue/info/property/same_shape/variable_shape/property.hpp"
#include "lue/test.hpp"


class Fixture:
    public lue::data_model::test::FileFixture
{

public:

    Fixture():
        FileFixture{"property.h5"},
        _filename{"property.h5"},
        _name{"my_property"},
        _datatype{lue::hdf5::NativeDatatypeTraits<int32_t>::type_id()},
        _rank{2},
        _file{std::make_unique<lue::hdf5::File>(
            lue::hdf5::create_file(_filename))},
        _property{std::make_unique<lue::data_model::same_shape::variable_shape::Property>(
            lue::data_model::same_shape::variable_shape::create_property(
                *_file, _name, _datatype, _rank))}
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

    auto rank() const
    {
        return _rank;
    }

private:

    std::string const _filename;
    std::string const _name;
    lue::hdf5::Datatype const _datatype;
    lue::data_model::Rank const _rank;
    std::unique_ptr<lue::hdf5::File> _file;
    std::unique_ptr<lue::data_model::same_shape::variable_shape::Property> _property;

};


BOOST_FIXTURE_TEST_CASE(create_property, Fixture)
{
    auto const& property = this->property();

    BOOST_CHECK_EQUAL(property.name(), name());
}
