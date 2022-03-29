#define BOOST_TEST_MODULE lue netcdf4 dataset
#include <boost/test/unit_test.hpp>
#include "lue/data_model/netcdf4/file.hpp"


namespace ln4 = lue::data_model::netcdf4;


BOOST_AUTO_TEST_CASE(default_construct)
{
    ln4::File file{};

    BOOST_CHECK(!file.is_open());
}
