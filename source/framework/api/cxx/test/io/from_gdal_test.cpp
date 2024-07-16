#define BOOST_TEST_MODULE lue framework api cxx io from_gdal
#include "lue/framework/api/cxx/io/from_gdal.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"


BOOST_AUTO_TEST_CASE(from_gdal)
{
    using Element = std::int32_t;
    using Array = lue::PartitionedArray<Element, 2>;

    [[maybe_unused]] std::string const name{"from_gdal.tif"};
    [[maybe_unused]] lue::Shape<lue::Count, 2> partition_shape{10, 10};

    {
        // TODO
        // lue::api::Field result = lue::api::from_gdal(name);

        // BOOST_CHECK(std::holds_alternative<Array>(result));
    }

    {
        // TODO
        // lue::api::Field result = lue::api::from_gdal(name, partition_shape);

        // BOOST_CHECK(std::holds_alternative<Array>(result));
    }
}
