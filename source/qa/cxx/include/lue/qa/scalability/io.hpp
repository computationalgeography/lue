#pragma once
#include "lue/qa/scalability/array_experiment.hpp"
#include <string>


namespace lue::qa {

    void save_results(ArrayExperiment const& experiment, std::string const& pathname);

}  // namespace lue::qa
