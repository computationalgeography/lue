#define BOOST_TEST_MODULE lue gdal driver_test
#include "lue/gdal/driver.hpp"
#include <boost/test/unit_test.hpp>


BOOST_AUTO_TEST_CASE(driver)
{
    namespace lgd = lue::gdal;

    lgd::register_gdal_drivers();

    {
        std::string const driver_name{"GTiff"};

        BOOST_CHECK_NO_THROW(lgd::driver(driver_name));
    }

    {
        std::string const driver_name{"MEM"};

        BOOST_CHECK_NO_THROW(lgd::driver(driver_name));
    }

    {
        std::string const driver_name{"hotseknots"};

        BOOST_CHECK_THROW(lgd::driver(driver_name), std::runtime_error);
    }
}
