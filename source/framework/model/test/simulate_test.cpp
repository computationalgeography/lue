#define BOOST_TEST_MODULE lue framework model simulate
#include "lue/framework/model/model.hpp"
#include "lue/framework/model/progressor.hpp"
#include "lue/framework/model/simulate.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"
// #include <hpx/config.hpp>
// #include <boost/test/unit_test.hpp>


namespace {

    class MyModel: public lue::Model
    {

        public:

            void preprocess([[maybe_unused]] lue::Count const sample_nr) final
            {
                ++preprocess_called;
            }


            void initialize() final
            {
                ++initialize_called;
            }


            hpx::shared_future<void> simulate([[maybe_unused]] lue::Count const time_step) final
            {
                ++simulate_called;

                return hpx::make_ready_future<void>();
            }


            void finalize() final
            {
                ++finalize_called;
            }


            void postprocess() final
            {
                ++postprocess_called;
            }


            lue::Count preprocess_called{0};
            lue::Count initialize_called{0};
            lue::Count simulate_called{0};
            lue::Count finalize_called{0};
            lue::Count postprocess_called{0};
    };


    class MyProgressor: public lue::Progressor
    {

        public:

            void preprocess([[maybe_unused]] lue::Count const sample_nr) final
            {
                ++preprocess_called;
            }


            void initialize() final
            {
                ++initialize_called;
            }


            void simulate([[maybe_unused]] lue::Count const time_step) final
            {
                ++simulate_called;
            }


            void finalize() final
            {
                ++finalize_called;
            }


            void postprocess() final
            {
                ++postprocess_called;
            }


            lue::Count preprocess_called{0};
            lue::Count initialize_called{0};
            lue::Count simulate_called{0};
            lue::Count finalize_called{0};
            lue::Count postprocess_called{0};
    };

}  // Anonymous namespace


BOOST_AUTO_TEST_CASE(run_deterministic)
{
    // Static model
    {
        MyModel model{};
        MyProgressor progressor{};
        lue::Count const nr_time_steps = 0;

        lue::run_deterministic(model, progressor, nr_time_steps);

        BOOST_CHECK_EQUAL(model.preprocess_called, 0);
        BOOST_CHECK_EQUAL(model.initialize_called, 1);
        BOOST_CHECK_EQUAL(model.simulate_called, nr_time_steps);
        BOOST_CHECK_EQUAL(model.finalize_called, 1);
        BOOST_CHECK_EQUAL(model.postprocess_called, 0);

        BOOST_CHECK_EQUAL(progressor.preprocess_called, 0);
        BOOST_CHECK_EQUAL(progressor.initialize_called, 1);
        BOOST_CHECK_EQUAL(progressor.simulate_called, nr_time_steps);
        BOOST_CHECK_EQUAL(progressor.finalize_called, 1);
        BOOST_CHECK_EQUAL(progressor.postprocess_called, 0);
    }

    // Dynamic model
    {
        MyModel model{};
        MyProgressor progressor{};
        lue::Count const nr_time_steps = 5;

        lue::run_deterministic(model, progressor, nr_time_steps);

        BOOST_CHECK_EQUAL(model.preprocess_called, 0);
        BOOST_CHECK_EQUAL(model.initialize_called, 1);
        BOOST_CHECK_EQUAL(model.simulate_called, nr_time_steps);
        BOOST_CHECK_EQUAL(model.finalize_called, 1);
        BOOST_CHECK_EQUAL(model.postprocess_called, 0);

        BOOST_CHECK_EQUAL(progressor.preprocess_called, 0);
        BOOST_CHECK_EQUAL(progressor.initialize_called, 1);
        BOOST_CHECK_EQUAL(progressor.simulate_called, nr_time_steps);
        BOOST_CHECK_EQUAL(progressor.finalize_called, 1);
        BOOST_CHECK_EQUAL(progressor.postprocess_called, 0);
    }
}


BOOST_AUTO_TEST_CASE(run_stochastic)
{
    // Static model
    {
        MyModel model{};
        MyProgressor progressor{};
        lue::Count const nr_samples = 5;
        lue::Count const nr_time_steps = 0;

        lue::run_stochastic(model, progressor, nr_samples, nr_time_steps);

        BOOST_CHECK_EQUAL(model.preprocess_called, nr_samples);
        BOOST_CHECK_EQUAL(model.initialize_called, nr_samples);
        BOOST_CHECK_EQUAL(model.simulate_called, nr_samples * nr_time_steps);
        BOOST_CHECK_EQUAL(model.finalize_called, nr_samples);
        BOOST_CHECK_EQUAL(model.postprocess_called, nr_samples);

        BOOST_CHECK_EQUAL(progressor.preprocess_called, nr_samples);
        BOOST_CHECK_EQUAL(progressor.initialize_called, nr_samples);
        BOOST_CHECK_EQUAL(progressor.simulate_called, nr_samples * nr_time_steps);
        BOOST_CHECK_EQUAL(progressor.finalize_called, nr_samples);
        BOOST_CHECK_EQUAL(progressor.postprocess_called, nr_samples);
    }

    // Dynamic model
    {
        MyModel model{};
        MyProgressor progressor{};
        lue::Count const nr_samples = 5;
        lue::Count const nr_time_steps = 5;

        lue::run_stochastic(model, progressor, nr_samples, nr_time_steps);

        BOOST_CHECK_EQUAL(model.preprocess_called, nr_samples);
        BOOST_CHECK_EQUAL(model.initialize_called, nr_samples);
        BOOST_CHECK_EQUAL(model.simulate_called, nr_samples * nr_time_steps);
        BOOST_CHECK_EQUAL(model.finalize_called, nr_samples);
        BOOST_CHECK_EQUAL(model.postprocess_called, nr_samples);

        BOOST_CHECK_EQUAL(progressor.preprocess_called, nr_samples);
        BOOST_CHECK_EQUAL(progressor.initialize_called, nr_samples);
        BOOST_CHECK_EQUAL(progressor.simulate_called, nr_samples * nr_time_steps);
        BOOST_CHECK_EQUAL(progressor.finalize_called, nr_samples);
        BOOST_CHECK_EQUAL(progressor.postprocess_called, nr_samples);
    }
}
