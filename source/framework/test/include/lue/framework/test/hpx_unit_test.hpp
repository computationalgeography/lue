#pragma once
#define BOOST_TEST_NO_MAIN
#define BOOST_TEST_ALTERNATIVE_INIT_API
#include <hpx/config.hpp>
#include <hpx/hpx_init.hpp>
#include <boost/cstdlib.hpp>
#include <boost/test/unit_test.hpp>


// https://stellar-group.github.io/hpx/docs/html/hpx.html#hpx.manual.applications
// http://www.boost.org/doc/libs/1_64_0/libs/test/doc/html/boost_test/adv_scenarios/entry_point_overview.html


int hpx_main(int argc, char* argv[])
{
    int test_status = boost::unit_test::unit_test_main(init_unit_test, argc, argv);

    int hpx_status = hpx::finalize();

    return test_status != boost::exit_success ? test_status : hpx_status;
}


int main(int argc, char* argv[])
// char* envp[])
{
    // Blocks until HPX runtime system exists

    // TODO Compare with CMake logic in LueMacro.cmake. We are handling
    //      this from the outside already and should not hardcode
    //      this here.
    std::vector<std::string> const cfg{// Use this number of threads per locality. This will utilize
                                       // the same number of cores per locality.
                                       "hpx.os_threads!=4"};

    hpx::init_params params{};
    params.cfg = {cfg};

    // TODO For some reason HPX complains about test specific arguments not
    //     being supported. It should skip unsupported arguments and pass them
    //     on to hpx_main. Not sure why it doesn't in this case.
    return hpx::init(argc, argv, params);
}
