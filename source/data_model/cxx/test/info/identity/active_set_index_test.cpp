#define BOOST_TEST_MODULE lue info identity active_set_index
#include "lue/core/tag.hpp"
#include "lue/info/identity/active_set_index.hpp"
#include "lue/test.hpp"
#include <boost/test/unit_test.hpp>


class Fixture: public lue::data_model::test::FileFixture
{

    public:

        Fixture():
            FileFixture{"value.h5"},
            _filename{"value.h5"},
            _file{std::make_unique<lue::hdf5::File>(lue::hdf5::create_file(_filename))},
            _value{std::make_unique<lue::data_model::ActiveSetIndex>(
                lue::data_model::create_active_set_index(*_file))}
        {
        }

        Fixture(Fixture const&) = delete;

        Fixture(Fixture&&) = delete;

        ~Fixture() override = default;

        Fixture& operator=(Fixture const&) = delete;

        Fixture& operator=(Fixture&&) = delete;

        auto& value()
        {
            return *_value;
        }

    private:

        std::string const _filename;
        lue::hdf5::Datatype const _datatype;
        std::unique_ptr<lue::hdf5::File> _file;
        std::unique_ptr<lue::data_model::ActiveSetIndex> _value;
};


BOOST_FIXTURE_TEST_CASE(create, Fixture)
{
    auto const& value = this->value();

    BOOST_CHECK_EQUAL(value.id().name(), lue::data_model::active_set_index_tag);
    BOOST_CHECK(value.memory_datatype() == lue::hdf5::Datatype{H5T_NATIVE_HSIZE});
    BOOST_CHECK_EQUAL(value.nr_arrays(), 0);
    BOOST_CHECK(value.array_shape().empty());
}


// Most functionality is inherited and tested elsewhere.
