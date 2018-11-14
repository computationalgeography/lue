#pragma once
#include <cstdlib>


namespace lue {
namespace benchmark {

class Environment
{

public:

                   Environment         (std::size_t count,
                                        std::size_t nr_threads,
                                        std::size_t work_size);

                   Environment         (std::size_t count,
                                        std::size_t nr_localities,
                                        std::size_t nr_threads,
                                        std::size_t work_size);

                   Environment         (Environment const&)=default;

                   Environment         (Environment&&)=default;

                   ~Environment        ()=default;

    Environment&   operator=           (Environment const&)=default;

    Environment&   operator=           (Environment&&)=default;

    std::size_t    count               () const;

    std::size_t    nr_localities       () const;

    std::size_t    nr_threads          () const;

    std::size_t    work_size           () const;

private:

    std::size_t const _count;

    std::size_t const _nr_localities;

    std::size_t const _nr_threads;

    std::size_t const _work_size;

};

}  // namespace benchmark
}  // namespace lue
