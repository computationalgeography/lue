#define BOOST_TEST_MODULE lue compound clock
#include <boost/test/unit_test.hpp>
#include "lue/hdf5/attribute.hpp"
#include "lue/hdf5/file.hpp"
#include "lue/compound/clock.hpp"


namespace {

class Fixture
{

public:

    Fixture(
        std::string const& pathname,
        bool const remove_file_upon_destruction=true)

        : _pathname(pathname),
          _remove_file_upon_destruction{remove_file_upon_destruction}

    {
        // Guarantees:
        // - File does not exist after setup
        remove_file();

        BOOST_REQUIRE(!lue::hdf5::file_exists(_pathname));
    }


    ~Fixture()
    {
        if(_remove_file_upon_destruction) {
            // Guarantees:
            // - File does not exist after teardown
            remove_file();

            BOOST_CHECK(!lue::hdf5::file_exists(_pathname));
        }
    }


private:

    std::string const _pathname;

    bool const     _remove_file_upon_destruction;

    void remove_file()
    {
        if(lue::hdf5::file_exists(_pathname)) {
            lue::hdf5::remove_file(_pathname);
        }
    }

};

}  // Anonymous namespace


BOOST_AUTO_TEST_CASE(write_clock)
{
    std::string const pathname = "write_clock.lue";

    std::string const attribute_name = "my_clock";
    auto const file_datatype = lue::compound::create_clock_file_datatype();
    auto const memory_datatype = lue::compound::create_clock_memory_datatype();
    lue::hdf5::Dataspace dataspace(::H5S_SCALAR);

    lue::time::Unit const unit = lue::time::Unit::second;
    lue::time::TickPeriodCount const nr_units = 10;

    Fixture f(pathname);

    {
        auto file = lue::hdf5::create_file(pathname);
        auto attribute = lue::hdf5::create_attribute(
            file.id(), attribute_name, file_datatype, dataspace);

        lue::compound::Clock clock_written(unit, nr_units);

        attribute.write(memory_datatype, clock_written);
    }

    {
        lue::hdf5::File file{pathname};
        auto attribute = lue::hdf5::Attribute(file.id(), attribute_name);

        lue::compound::Clock clock_read;

        attribute.read(memory_datatype, clock_read);

        BOOST_CHECK_EQUAL(
            clock_read._unit_name,
            lue::time::UnitTraits<lue::time::Unit::second>::name());
        BOOST_CHECK_EQUAL(clock_read._nr_units, nr_units);
    }
}
