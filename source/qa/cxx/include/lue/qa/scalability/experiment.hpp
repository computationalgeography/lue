#pragma once
#include "lue/qa/scalability/run.hpp"
#include <vector>


namespace lue::qa {

    using Count = std::size_t;

    using Runs = std::vector<Run>;


    class Experiment:
        public Run
    {

        public:

            Experiment(
                Count const nr_workers);

            Experiment(Experiment const&)=default;

            Experiment(Experiment&&)=default;

            ~Experiment()=default;

            Experiment& operator=(Experiment const&)=default;

            Experiment& operator=(Experiment&&)=default;

            Count nr_workers() const;

            void add(Run const& run);

            Runs const& runs() const;

        private:

            Count _nr_workers;

            Runs _runs;

    };

}  // namespace lue::qa
