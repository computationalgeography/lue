#define BOOST_TEST_MODULE lue info space mobile_space_box
#include "lue/core/tag.hpp"
#include "lue/info/space/mobile_space_box.hpp"
#include "lue/test.hpp"
#include <boost/test/unit_test.hpp>


class Fixture: public lue::data_model::test::FileFixture
{

    public:

        Fixture():
            FileFixture{"value.h5"},
            _filename{"value.h5"},
            _datatype{lue::hdf5::NativeDatatypeTraits<int32_t>::type_id()},
            _rank{2},
            _file{std::make_unique<lue::hdf5::File>(lue::hdf5::create_file(_filename))},
            _value{std::make_unique<lue::data_model::MobileSpaceBox>(
                lue::data_model::create_mobile_space_box(*_file, _datatype, _rank))}
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
        lue::data_model::Rank const _rank;
        std::unique_ptr<lue::hdf5::File> _file;
        std::unique_ptr<lue::data_model::MobileSpaceBox> _value;
};


BOOST_FIXTURE_TEST_CASE(create, Fixture)
{
    namespace lh5 = lue::hdf5;
    namespace ldm = lue::data_model;

    auto const& value = this->value();

    BOOST_CHECK_EQUAL(value.id().name(), ldm::coordinates_tag);
    BOOST_CHECK(value.memory_datatype() == datatype());
    BOOST_CHECK(value.file_datatype() == lh5::file_datatype(datatype()));
    BOOST_CHECK_EQUAL(value.nr_arrays(), 0);
    BOOST_CHECK(value.array_shape() == lh5::Shape{static_cast<lh5::Shape::value_type>(2 * rank())});
}


// Most functionality is inherited and tested elsewhere.
