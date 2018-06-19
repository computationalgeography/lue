#define BOOST_TEST_MODULE lue hdf5 attribute
#include <boost/test/unit_test.hpp>
#include "lue/hdf5/attribute.hpp"
#include "lue/hdf5/file.hpp"


namespace {

class Fixture
{

public:

    Fixture(
        std::string const& dataset_name)

        : _filename(dataset_name)

    {
        // Guarantees:
        // - File does not exist after setup
        remove_file();

        BOOST_REQUIRE(!lue::hdf5::file_exists(_filename));
    }


    ~Fixture()
    {
        // Guarantees:
        // - File does not exist after teardown
        remove_file();

        BOOST_CHECK(!lue::hdf5::file_exists(_filename));
    }


private:

    std::string const _filename;

    void remove_file()
    {
        if(lue::hdf5::file_exists(_filename)) {
            lue::hdf5::remove_file(_filename);
        }
    }

};

}  // Anonymous namespace


BOOST_AUTO_TEST_CASE(string_attribute)
{
    std::string const filename = "string_attribute.lue";
    Fixture f(filename);

    auto const file = lue::hdf5::create_file(filename);
    std::string const attribute_name = "my_attribute";
    std::string const attribute_value = "my_attribute_value";

    {
        auto attribute = lue::hdf5::create_attribute(file.id(),
            attribute_name, attribute_value);

        attribute.write<std::string>(attribute_value);
    }

    {
        auto attribute = lue::hdf5::Attribute(file.id(), attribute_name);

        BOOST_CHECK_EQUAL(attribute.read<std::string>(), attribute_value);
    }
}
