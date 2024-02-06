#pragma once
#define BOOST_TEST_NO_MAIN
#define BOOST_TEST_ALTERNATIVE_INIT_API
#include "lue/gdal.hpp"
#include <hpx/config.hpp>
#include <hpx/hpx_init.hpp>
#include <boost/cstdlib.hpp>
#include <boost/test/unit_test.hpp>


// https://stellar-group.github.io/hpx/docs/html/hpx.html#hpx.manual.applications
// ħttps://www.boost.org/doc/libs/1_84_0/libs/test/doc/html/boost_test/adv_scenarios/entry_point_overview.html


int hpx_main(int argc, char* argv[])
{
    int test_status = boost::unit_test::unit_test_main(init_unit_test, argc, argv);

    int hpx_status = hpx::finalize();

    return test_status != boost::exit_success ? test_status : hpx_status;
}


int main(int argc, char* argv[])
{
    // Make sure that processes in tests that depend on GDAL drivers have access to the GDAL
    // drivers
    lue::gdal::register_gdal_drivers();

    std::vector<std::string> const cfg{};

    hpx::init_params params{};
    params.cfg = {cfg};

    return hpx::init(argc, argv, params);
}
