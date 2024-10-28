#define BOOST_TEST_MODULE lue qa scalability instrument array_experiment
#include "lue/qa/scalability/instrument/array_experiment.hpp"
#include "lue/stream.hpp"
#include <boost/test/included/unit_test.hpp>


BOOST_AUTO_TEST_CASE(use_case_01)
{
    lue::qa::Count const nr_workers{5};
    lue::qa::Shape const array_shape{60, 40};
    lue::qa::Shape const partition_shape{10, 10};
    lue::qa::ArrayExperiment array_experiment{nr_workers, array_shape, partition_shape};

    BOOST_CHECK_EQUAL(array_experiment.nr_workers(), nr_workers);
    BOOST_CHECK_EQUAL(array_experiment.array_shape(), array_shape);
    BOOST_CHECK_EQUAL(array_experiment.partition_shape(), partition_shape);

    array_experiment.start();

    lue::qa::Count const nr_runs{3};

    for (lue::qa::Count run_idx = 0; run_idx < nr_runs; ++run_idx)
    {
        lue::qa::Run run{};

        run.start();
        run.stop();

        array_experiment.add(run);
    }

    array_experiment.stop();

    BOOST_CHECK_EQUAL(std::size(array_experiment.runs()), nr_runs);
}
