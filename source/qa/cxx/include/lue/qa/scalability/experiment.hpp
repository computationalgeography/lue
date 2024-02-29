#pragma once
#include "lue/qa/scalability/run.hpp"
#include <vector>


namespace lue::qa {

    //! Type for representing counts
    using Count = std::size_t;

    //! Type for representing collections of runs
    using Runs = std::vector<Run>;


    /*!
        @brief      The Experiment class represents the results of performing one or more experimental runs

        During a scalability experiment, software is being executed one or multiple
        times. Executing it multiple times allows for the calculation of statistics, like the
        mean duration a run took.

        Typical pattern:

        \code
        Experiment experiment(nr_workers);
        experiment.start();

        for(Count run_idx = 0; run_idx < nr_runs; ++run_idx)
        {
            Run run{};

            // Do any preprocessing before this point. Also, be sure to wait
            // for any asynchronous work started to finish.
            run.start();

            // The actual work to measure the runtime of
            do_something();

            // Be sure to first wait for any asynchronous work started to finish
            run.stop();
            experiment.add(run);
        }

        experiment.stop();
        save_results(experiment, result_pathname);
        \endcode
    */
    class Experiment: public Run
    {

        public:

            Experiment(Count nr_workers);

            Experiment(Experiment const&) = default;

            Experiment(Experiment&&) = default;

            ~Experiment() = default;

            auto operator=(Experiment const&) -> Experiment& = default;

            auto operator=(Experiment&&) -> Experiment& = default;

            [[nodiscard]] auto nr_workers() const -> Count;

            void add(Run const& run);

            [[nodiscard]] auto runs() const -> Runs const&;

        private:

            Count _nr_workers;

            Runs _runs;
    };

}  // namespace lue::qa
