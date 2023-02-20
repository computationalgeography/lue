#define BOOST_TEST_MODULE lue info identity object_id
#include "lue/core/tag.hpp"
#include "lue/info/identity/object_id.hpp"
#include "lue/test.hpp"
#include <boost/test/unit_test.hpp>


class Fixture: public lue::data_model::test::FileFixture
{

    public:

        Fixture():
            FileFixture{"value.h5"},
            _filename{"value.h5"},
            _file{std::make_unique<lue::hdf5::File>(lue::hdf5::create_file(_filename))},
            _object_id{std::make_unique<lue::data_model::ObjectID>(lue::data_model::create_object_id(*_file))}
        {
        }

        Fixture(Fixture const&) = delete;

        Fixture(Fixture&&) = delete;

        ~Fixture() override = default;

        Fixture& operator=(Fixture const&) = delete;

        Fixture& operator=(Fixture&&) = delete;

        auto& object_id()
        {
            return *_object_id;
        }

    private:

        std::string const _filename;
        std::unique_ptr<lue::hdf5::File> _file;
        std::unique_ptr<lue::data_model::ObjectID> _object_id;
};


BOOST_FIXTURE_TEST_CASE(create, Fixture)
{
    auto const& object_id = this->object_id();

    BOOST_CHECK_EQUAL(object_id.id().name(), lue::data_model::object_id_tag);
    BOOST_CHECK(object_id.memory_datatype() == lue::hdf5::Datatype{H5T_NATIVE_HSIZE});
    BOOST_CHECK_EQUAL(object_id.nr_arrays(), 0);
    BOOST_CHECK(object_id.array_shape().empty());
    BOOST_CHECK_EQUAL(object_id.nr_objects(), 0);
}


// Most functionality is inherited and tested elsewhere.
