#pragma once
#include <cstdlib>


namespace lue {
namespace benchmark {

class Environment
{

public:

    explicit       Environment         (std::size_t nr_threads);

    explicit       Environment         (std::size_t nr_localities,
                                        std::size_t nr_threads);

                   Environment         (Environment const&)=default;

                   Environment         (Environment&&)=default;

                   ~Environment        ()=default;

    Environment&   operator=           (Environment const&)=default;

    Environment&   operator=           (Environment&&)=default;

    std::size_t    nr_localities       () const;

    std::size_t    nr_threads          () const;

private:

    std::size_t const _nr_localities;

    std::size_t const _nr_threads;

};

}  // namespace benchmark
}  // namespace lue
