#pragma once
#include "lue/framework/core/assert.hpp"
#include "lue/framework/core/define.hpp"
#include <hpx/future.hpp>
#include <hpx/synchronization/sliding_semaphore.hpp>


namespace lue {
    namespace detail {

        auto attach_signaller = [](hpx::sliding_semaphore& semaphore,
                                   auto&& previous_state,
                                   auto&& current_state,
                                   Count const lower_limit)
        {
            // To prevent states to overtake each other, we order them explicitly here

            // Once the previous state is ready ...
            return previous_state.then(
                [&semaphore,
                 current_state = std::forward<decltype(current_state)>(current_state),
                 lower_limit]([[maybe_unused]] auto const& previous_state)
                {
                    // ... and once the current state is ready...
                    return current_state.then(
                        [&semaphore, lower_limit](auto const& current_state)
                        {
                            HPX_UNUSED(current_state);  // Silence compiler in non-Debug build
                            lue_hpx_assert(current_state.is_ready());

                            // ... notify the semaphore about the new lower limit
                            semaphore.signal(lower_limit);
                        });
                });
        };

    }  // namespace detail


    /*!
        @brief      Execute @a model iteratively, from time step @a first_time_step up and until time step
                    @a last_time_step
        @tparam     Model Type of the model
        @tparam     Progressor Type of the progressor
        @param      model Model to execute
        @param      progressor Progressor to report progress to
        @param      first_time_step First time step to execute
        @param      last_time_step Last time step to execute
        @param      rate_limit Maximum number of time steps to execute "at the same time"
        @exception  .

        A rate limit can be used to put a brake on the speed with which model time steps are simulated.
        Without it, the system may be flooded with too many tasks.
    */
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

        Count const nr_time_steps{last_time_step - first_time_step + 1};
        bool const rate_limit_used{rate_limit > 0};
        Count const default_rate_limit{nr_time_steps};
        rate_limit = rate_limit_used ? rate_limit : default_rate_limit;

        std::int64_t const max_difference{rate_limit};
        hpx::sliding_semaphore semaphore{max_difference, first_time_step};

        // A type to represent the simulation's "state" after a single time step. It is assumed that once this
        // state becomes ready, that the tasks for the corresponding time step have all finished.
        using State = hpx::shared_future<void>;

        {
            State previous_state{hpx::make_ready_future<void>()};
            Count current_time_step{first_time_step};

            for (Index time_step_idx = 0; time_step_idx < nr_time_steps; ++time_step_idx, ++current_time_step)
            {
                State current_state = simulate(model, current_time_step);

                // Every rate_limit time steps, attach an additional continuation which will
                // trigger the semaphore once the computation has reached this point. This informs the
                // semaphore that "old" states have finished.
                // if (((time_step_idx + 1) % rate_limit) == 0)
                // {
                previous_state = detail::attach_signaller(
                    semaphore, std::move(previous_state), std::move(current_state), current_time_step);
                // }

                // Set the new upper limit. Wait if necessary. Continue if / once the difference
                // between the lower and upper limits is not larger than max_difference set.
                semaphore.wait(current_time_step);

                if (current_time_step == last_time_step)
                {
                    // This will make sure this function does not return before the last state is
                    // ready. Returning too early results in dangling references to the semaphore.
                    // Wait explicitly here, because the semaphore only waits conditionally. The
                    // last state may or may not be already ready.
                    previous_state.wait();
                }

                simulate(progressor, current_time_step);
            }
        }
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
