#define BOOST_TEST_MODULE lue framework api c local_operation
#include "lue/framework/api/c/local_operation.h"
#include "lue/framework/test/hpx_unit_test.hpp"


BOOST_AUTO_TEST_CASE(add_raster_raster)
{
    BOOST_CHECK(true);
}


// TODO
// close_to
// equal_to
// greater_than
// greater_than_equal_to
// less_than
// logical_and
// logical_exclusive_or
// logical_inclusive_or
// logical_not


// #include "lue/framework/api/c/local_operation.h"
//
//
// int main()
// {
//     // TODO Start HPX runtime
//
//     // TODO
//     Field* field1 = 0;  //  = uniform(...);
//     Field* field2 = 0;  //  = uniform(...);
//
//     {
//         Field* result = add(field1, field2);
//
//         // TODO Test result
//
//         destruct(result);
//     }
//
//     destruct(field1);
//     destruct(field2);
//
//     // TODO Stop HPX runtime
// }
