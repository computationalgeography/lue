#include "lue/qa/scalability/experiment.hpp"


namespace lue::qa {

    /*!
        @brief      Construct and instance for an experiment using @a nr_workers workers
    */
    Experiment::Experiment(Count const nr_workers):

        _nr_workers{nr_workers}

    {
    }


    /*!
        @brief      Return the number of workers this experiment is for
    */
    auto Experiment::nr_workers() const -> Count
    {
        return _nr_workers;
    }


    /*!
        @brief      Add (save) the results of an experimental run
    */
    void Experiment::add(Run const& run)
    {
        _runs.push_back(run);
    }


    /*!
        @brief      Return the collection with the results of the experimental runs added
    */
    auto Experiment::runs() const -> Runs const&
    {
        return _runs;
    }

}  // namespace lue::qa
