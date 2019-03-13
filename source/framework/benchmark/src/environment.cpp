#include "lue/framework/benchmark/environment.hpp"


namespace lue {
namespace benchmark {

Environment::Environment(
    std::size_t const count,
    std::size_t const nr_threads,
    std::size_t const work_size):

    Environment{count, 1, nr_threads, work_size}

{
}


Environment::Environment(
    std::size_t const count,
    std::size_t const nr_localities,
    std::size_t const nr_threads,
    std::size_t const work_size):

    _count{count},
    _nr_localities{nr_localities},
    _nr_threads{nr_threads},
    _work_size{work_size}

{
}


std::size_t Environment::count() const
{
    return _count;
}


std::size_t Environment::nr_localities() const
{
    return _nr_localities;
}


std::size_t Environment::nr_threads() const
{
    return _nr_threads;
}


std::size_t Environment::work_size() const
{
    return _work_size;
}

}  // namespace benchmark
}  // namespace lue
