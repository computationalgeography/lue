#define BOOST_TEST_MODULE lue framework model model
#include "lue/framework/model/model.hpp"
#include <boost/test/included/unit_test.hpp>


BOOST_AUTO_TEST_CASE(default_construct)
{
    // It must be possible to default construct the model base class
    lue::Model model{};
}
