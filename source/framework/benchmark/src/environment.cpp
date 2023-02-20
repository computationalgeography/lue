#include "lue/framework/benchmark/environment.hpp"


namespace lue {
    namespace benchmark {

        // Environment::Environment(
        //     // std::string const& system_name,
        //     std::size_t const count,
        //     std::size_t const nr_threads,
        //     std::optional<std::size_t> const& max_tree_depth):
        //     // std::size_t const work_size):
        //
        //     Environment{/* system_name, */ count, 1, nr_threads, max_tree_depth} // , work_size}
        //
        // {
        // }
        //
        //
        // Environment::Environment(
        //     // std::string const& system_name,
        //     std::size_t const count,
        //     std::size_t const nr_localities,
        //     std::size_t const nr_threads,
        //     std::optional<std::size_t> const& max_tree_depth):
        //     // std::size_t const work_size):
        //
        //     // _system_name{system_name},
        //     _count{count},
        //     _nr_localities{nr_localities},
        //     _nr_threads{nr_threads},
        //     _max_tree_depth{max_tree_depth}  // ,
        //     // _work_size{work_size}
        //
        // {
        // }


        Environment::Environment(
            std::size_t const count,
            std::size_t const nr_workers,
            std::optional<std::size_t> const& max_tree_depth):

            _count{count},
            _nr_workers{nr_workers},
            _max_tree_depth{max_tree_depth}

        {
        }


        // std::string const& Environment::system_name() const
        // {
        //     return _system_name;
        // }


        std::size_t Environment::count() const
        {
            return _count;
        }


        std::size_t Environment::nr_workers() const
        {
            return _nr_workers;
        }


        // std::size_t Environment::nr_localities() const
        // {
        //     return _nr_localities;
        // }
        //
        //
        // std::size_t Environment::nr_threads() const
        // {
        //     return _nr_threads;
        // }


        std::optional<std::size_t> const& Environment::max_tree_depth() const
        {
            return _max_tree_depth;
        }


        // std::size_t Environment::work_size() const
        // {
        //     return _work_size;
        // }

    }  // namespace benchmark
}  // namespace lue
