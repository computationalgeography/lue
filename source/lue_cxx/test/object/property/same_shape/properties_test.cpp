#define BOOST_TEST_MODULE lue object property same_shape properties
#include <boost/test/unit_test.hpp>
#include "lue/object/property/same_shape/properties.hpp"
#include "lue/test.hpp"


class Fixture:
    public lue::test::FileFixture
{

public:

    Fixture():
        FileFixture{"my_file.h5"},
        _filename{"my_file.h5"},
        _file{std::make_unique<lue::hdf5::File>(
            lue::hdf5::create_file(_filename))}  // ,
        // _properties{std::make_unique<lue::same_shape::Properties>(
        //     lue::same_shape::create_properties(
        //         *_file))}
    {
    }

    ~Fixture()
    {
    }

    // auto& value()
    // {
    //     return *_properties;
    // }

private:

    std::string const _filename;
    std::unique_ptr<lue::hdf5::File> _file;
    // std::unique_ptr<lue::same_shape::Properties> _properties;

};


BOOST_FIXTURE_TEST_CASE(create_value, Fixture)
{
    // TODO Use Properties class to create this thing

    // auto const& properties = this->properties();


    // BOOST_CHECK(value.memory_datatype() == datatype());
    // BOOST_CHECK(
    //     value.file_datatype() == lue::hdf5::file_datatype(datatype()));
    // BOOST_CHECK_EQUAL(value.nr_arrays(), 0);
    // BOOST_CHECK(value.array_shape() == array_shape());
}
