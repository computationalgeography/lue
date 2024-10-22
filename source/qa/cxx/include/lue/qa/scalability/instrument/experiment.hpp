#pragma once
#include "lue/qa/scalability/instrument/run.hpp"
#include <vector>


namespace lue::qa {

    //! Type for representing counts
    using Count = std::size_t;

    //! Type for representing collections of runs
    using Runs = std::vector<Run>;


    /*!
        @brief      The Experiment class represents the results of performing one or more experimental runs

        During a scalability experiment, software is being executed once or multiple times. Executing it
        multiple times allows for the calculation of statistics, like the mean duration a run took.

        This class inherits from Run, so it is an experimental run itself. This allows the duration of the
        whole "experiment run" to be measured.

        Typical usage pattern:

        @code{.cpp}
        Experiment experiment(nr_workers);
        experiment.start();

        for(Count run_idx = 0; run_idx < nr_runs; ++run_idx)
        {
            Run run{};

            // Do any preprocessing before this point. Also, be sure to wait
            // for any asynchronous work started to finish.
            // ...

            // Now start the run, measuring its duration
            run.start();

            // The actual work to measure the runtime of
            do_something();

            // Be sure to first wait for any asynchronous work started to finish
            // ...

            // Now stop this run and store the results. Do any post processing after this point.
            run.stop();
            experiment.add(run);
        }

        experiment.stop();
        save_results(experiment, result_pathname);
        @endcode
    */
    class Experiment: public Run
    {

        public:

            Experiment(Count nr_workers);

            Experiment(Experiment const&) = default;

            Experiment(Experiment&&) = default;

            ~Experiment() override = default;

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
