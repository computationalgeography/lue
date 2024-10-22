#define BOOST_TEST_MODULE lue qa scalability instrument experiment
#include "lue/qa/scalability/instrument/experiment.hpp"
#include "lue/stream.hpp"
#include <boost/test/included/unit_test.hpp>


BOOST_AUTO_TEST_CASE(use_case_01)
{
    lue::qa::Count const nr_workers{5};
    lue::qa::Experiment experiment{nr_workers};

    BOOST_CHECK_EQUAL(experiment.nr_workers(), nr_workers);

    experiment.start();

    lue::qa::Count const nr_runs{3};

    for (lue::qa::Count run_idx = 0; run_idx < nr_runs; ++run_idx)
    {
        lue::qa::Run run{};

        run.start();
        run.stop();

        experiment.add(run);
    }

    experiment.stop();

    BOOST_CHECK_EQUAL(std::size(experiment.runs()), nr_runs);
}
