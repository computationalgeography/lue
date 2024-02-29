#include "lue/qa/scalability/experiment.hpp"


namespace lue::qa {

    Experiment::Experiment(Count const nr_workers):

        Run{},
        _nr_workers{nr_workers}

    {
    }


    auto Experiment::nr_workers() const -> Count
    {
        return _nr_workers;
    }


    void Experiment::add(Run const& run)
    {
        _runs.push_back(run);
    }


    auto Experiment::runs() const -> Runs const&
    {
        return _runs;
    }

}  // namespace lue::qa
