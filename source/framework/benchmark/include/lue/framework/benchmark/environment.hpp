#pragma once
#include <cstdlib>
#include <optional>
#include <string>


namespace lue {
namespace benchmark {

class Environment
{

public:

                   // Environment         (// std::string const& system_name,
                   //                      std::size_t count,
                   //                      std::size_t nr_threads,
                   //                      std::optional<std::size_t> const&
                   //                          max_tree_depth);
                   //                      // std::size_t work_size);

                   // Environment         (// std::string const& system_name,
                   //                      std::size_t count,
                   //                      std::size_t nr_localities,
                   //                      std::size_t nr_threads,
                   //                      std::optional<std::size_t> const&
                   //                          max_tree_depth);
                   //                      // std::size_t work_size);

                   Environment         (std::size_t count,
                                        std::size_t nr_workers,
                                        std::optional<std::size_t> const&
                                            max_tree_depth);

                   Environment         (Environment const&)=default;

                   Environment         (Environment&&)=default;

                   ~Environment        ()=default;

    Environment&   operator=           (Environment const&)=default;

    Environment&   operator=           (Environment&&)=default;

    // std::string const& system_name     () const;

    std::size_t    count               () const;

    std::size_t    nr_workers          () const;

    // std::size_t    nr_localities       () const;

    // std::size_t    nr_threads          () const;

    std::optional<std::size_t> const&
                   max_tree_depth      () const;

    // std::size_t    work_size           () const;

private:

    // std::string const _system_name;

    std::size_t const _count;

    std::size_t const _nr_workers;

    // std::size_t const _nr_localities;

    // std::size_t const _nr_threads;

    std::optional<std::size_t> const _max_tree_depth;

    // std::size_t const _work_size;

};

}  // namespace benchmark
}  // namespace lue
