// #include "lue/framework/api/c/io.h"
//
//
// int main()
// {
//     // TODO Start HPX runtime
//
//     {
//         Field* result = from_gdal("x.tif");
//
//         // TODO Test result
//
//         destruct(result);
//     }
//
//     // TODO Stop HPX runtime
// }


#define BOOST_TEST_MODULE lue framework api c io
#include "lue/framework/api/c/io.h"
#include "lue/framework/test/hpx_unit_test.hpp"


BOOST_AUTO_TEST_CASE(round_trip)
{
    BOOST_CHECK(false);

    // using Element = std::int32_t;
    // using Array = lue::PartitionedArray<Element, 2>;
    // using Shape = lue::ShapeT<Array>;

    // Shape const array_shape{1, 1};
    // Shape const partition_shape{1, 1};

    // lue::api::Field const fill_value = Element{};

    // lue::api::Field field = lue::api::create_array(array_shape, partition_shape, fill_value);

    // BOOST_CHECK(std::holds_alternative<Array>(field));

    // std::string const name{"api_cxx_io_round_trip.tif"};

    // lue::api::to_gdal(field, name).wait();

    // lue::api::Field result = lue::api::from_gdal(name);

    // BOOST_CHECK(std::holds_alternative<Array>(result));
}
