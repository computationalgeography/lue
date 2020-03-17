#define BOOST_TEST_MODULE lue framework model model
#include <boost/test/unit_test.hpp>
#include "lue/framework/model/model.hpp"


BOOST_AUTO_TEST_CASE(default_construct)
{
    // It must be possible to default construct the model base class
    lue::Model model{};
}
