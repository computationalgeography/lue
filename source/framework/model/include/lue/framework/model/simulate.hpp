#pragma once
#include "lue/framework/core/assert.hpp"
#include "lue/framework/core/define.hpp"
#include <hpx/synchronization/sliding_semaphore.hpp>


namespace lue {

    template<typename Model, typename Progressor>
    void simulate(
        Model& model,
        Progressor& progressor,
        Count const first_time_step,
        Count const last_time_step,
        Count rate_limit = 0)
    {
        lue_hpx_assert(rate_limit >= 0);
        lue_hpx_assert(first_time_step <= last_time_step);

        rate_limit = rate_limit > 0 ? rate_limit : last_time_step - first_time_step + 1;

        std::int64_t const max_difference{first_time_step + rate_limit - 1};
        std::int64_t const lower_limit{first_time_step};
        hpx::sliding_semaphore semaphore{max_difference, lower_limit};

        hpx::shared_future<void> future{};

        for (Count time_step = first_time_step; time_step <= last_time_step; ++time_step)
        {
            future = simulate(model, time_step);

            // Every rate_limit time steps, attach an additional continuation which will
            // trigger the semaphore once computation has reached this point
            if ((time_step % rate_limit) == 0)
            {
                future.then(
                    [&semaphore, time_step](hpx::shared_future<void> const& future)
                    {
                        HPX_UNUSED(future);
                        lue_hpx_assert(future.is_ready());

                        // The future is ready. Now set the new lower limit.
                        semaphore.signal(time_step);
                    });
            }

            // Set the new upper limit. Wait if necessary. Continue if / once the difference
            // between the lower and upper limits if not larger than rate_limit.
            semaphore.wait(time_step);

            simulate(progressor, time_step);
        }

        // Wait for the last result to be ready. Otherwise the process in many cases will
        // not finish.
        future.wait();
    }


    template<typename Model, typename Progressor>
    void run_deterministic(
        Model& model, Progressor& progressor, Count const nr_time_steps, Count const rate_limit = 0)
    {
        initialize(progressor);
        initialize(model);

        if (nr_time_steps > 0)
        {
            simulate(model, progressor, 1, nr_time_steps, rate_limit);
        }

        finalize(model);
        finalize(progressor);
    }


    template<typename Model, typename Progressor>
    void run_stochastic(
        Model& model,
        Progressor& progressor,
        Count const nr_samples,
        Count const nr_time_steps,
        Count const rate_limit = 0)
    {
        for (Count sample_nr = 1; sample_nr <= nr_samples; ++sample_nr)
        {
            preprocess(progressor, sample_nr);
            preprocess(model, sample_nr);

            run_deterministic(model, progressor, nr_time_steps, rate_limit);

            postprocess(model);
            postprocess(progressor);
        }
    }

}  // namespace lue
