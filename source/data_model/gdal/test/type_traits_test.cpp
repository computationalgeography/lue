#define BOOST_TEST_MODULE lue gdal driver_test
#include "lue/gdal/type_traits.hpp"
#include <boost/test/included/unit_test.hpp>


BOOST_AUTO_TEST_CASE(data_type_v)
{
    BOOST_CHECK_EQUAL(lue::gdal::data_type_v<std::int32_t>, GDT_Int32);
}
