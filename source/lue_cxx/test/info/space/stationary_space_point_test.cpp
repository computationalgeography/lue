#define BOOST_TEST_MODULE lue info space stationary_space_point
#include <boost/test/unit_test.hpp>
#include "lue/info/space/stationary_space_point.hpp"
#include "lue/core/tag.hpp"
#include "lue/test.hpp"


class Fixture:
    public lue::test::FileFixture
{

public:

    Fixture():
        FileFixture{"value.h5"},
        _filename{"value.h5"},
        _datatype{lue::hdf5::NativeDatatypeTraits<int32_t>::type_id()},
        _rank{2},
        _file{std::make_unique<lue::hdf5::File>(
            lue::hdf5::create_file(_filename))},
        _value{std::make_unique<lue::StationarySpacePoint>(
            lue::create_stationary_space_point(*_file, _datatype, _rank))}
    {
    }

    ~Fixture()
    {
    }

    auto& value()
    {
        return *_value;
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
    lue::hdf5::Datatype const _datatype;
    lue::Rank const _rank;
    std::unique_ptr<lue::hdf5::File> _file;
    std::unique_ptr<lue::StationarySpacePoint> _value;

};


BOOST_FIXTURE_TEST_CASE(create, Fixture)
{
    using namespace lue;

    auto const& value = this->value();

    BOOST_CHECK_EQUAL(value.id().name(), coordinates_tag);
    BOOST_CHECK(value.memory_datatype() == datatype());
    BOOST_CHECK(
        value.file_datatype() == lue::hdf5::file_datatype(datatype()));
    BOOST_CHECK_EQUAL(value.nr_arrays(), 0);
    BOOST_CHECK(value.array_shape() == hdf5::Shape{
        static_cast<hdf5::Shape::value_type>(rank())});
}


// Most functionality is inherited and tested elsewhere.
