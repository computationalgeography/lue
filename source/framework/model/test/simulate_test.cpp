#define BOOST_TEST_MODULE lue framework model simulate
#include <hpx/config.hpp>
#include <boost/test/unit_test.hpp>
#include "lue/framework/model/model.hpp"
#include "lue/framework/model/simulate.hpp"


BOOST_AUTO_TEST_CASE(default_constructed_model)
{
    // This should compile and work fine

    lue::Model model{};
    lue::Count nr_time_steps;

    {
        nr_time_steps = 0;
        lue::simulate(model, nr_time_steps);
    }

    {
        nr_time_steps = 1;
        lue::simulate(model, nr_time_steps);
    }
}
