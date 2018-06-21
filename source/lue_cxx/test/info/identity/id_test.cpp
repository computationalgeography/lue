#define BOOST_TEST_MODULE lue info identity id
#include <boost/test/unit_test.hpp>
#include "lue/info/identity/id.hpp"
#include "lue/core/tag.hpp"
#include "lue/test.hpp"


class Fixture:
    public lue::test::FileFixture
{

public:

    Fixture()
        : FileFixture{"value.h5"},
          _filename{"value.h5"},
          _file{std::make_unique<lue::hdf5::File>(
            lue::hdf5::create_file(_filename))},
          _id{std::make_unique<lue::info::ID>(
            lue::info::create_id(*_file))}
    {
    }

    ~Fixture()
    {
    }

    auto& id()
    {
        return *_id;
    }

private:

    std::string const _filename;
    std::unique_ptr<lue::hdf5::File> _file;
    std::unique_ptr<lue::info::ID> _id;

};


BOOST_FIXTURE_TEST_CASE(create, Fixture)
{
    auto const& id = this->id();

    BOOST_CHECK_EQUAL(id.id().name(), lue::id_tag);
    BOOST_CHECK(
        id.memory_datatype() == lue::hdf5::Datatype{H5T_NATIVE_HSIZE});
    BOOST_CHECK_EQUAL(id.nr_arrays(), 0);
    BOOST_CHECK(id.array_shape() == lue::hdf5::Shape{});
    BOOST_CHECK_EQUAL(id.nr_objects(), 0);
}


// Most functionality is inherited and tested elsewhere.
