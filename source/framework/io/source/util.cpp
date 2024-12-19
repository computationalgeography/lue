#include "lue/framework/io/util.hpp"


namespace lue::detail {

    /// auto when_all_get(std::vector<hpx::future<void>>&& futures) -> hpx::future<void>
    /// {
    ///     // When waiting on the result of asynchronous actions returning
    ///     // nothing (hpx::future<void>), any exception thrown will evaporate if
    ///     // we don't get the 'value'. Here we 'wait' for all futures to become
    ///     // ready, and then get the 'value' to let any exception be rethrown.
    ///     // The caller, in turn, has to get the 'value' of the future we return!
    ///     return hpx::when_all(futures.begin(), futures.end())
    ///         .then(

    ///             [](hpx::future<std::vector<hpx::future<void>>> futures_ftr)
    ///             {
    ///                 auto futures{futures_ftr.get()};
    ///                 std::for_each(
    ///                     futures.begin(), futures.end(), [](hpx::future<void>& future) { future.get(); });
    ///             }

    ///         );
    /// }

}  // namespace lue::detail
