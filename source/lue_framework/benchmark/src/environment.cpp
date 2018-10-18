#include "lue/framework/benchmark/environment.hpp"


namespace lue {
namespace benchmark {

Environment::Environment(
    std::size_t const nr_threads):

    _nr_localities{1},
    _nr_threads{nr_threads}

{
}


Environment::Environment(
    std::size_t const nr_localities,
    std::size_t const nr_threads):

    _nr_localities{nr_localities},
    _nr_threads{nr_threads}

{
}


std::size_t Environment::nr_localities() const
{
    return _nr_localities;
}


std::size_t Environment::nr_threads() const
{
    return _nr_threads;
}

}  // namespace benchmark
}  // namespace lue
