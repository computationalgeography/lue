#include "lue/framework/io/lue.hpp"
#include "lue/framework/core/define.hpp"

// #include <hpx/iostream.hpp>
// #include <format>


namespace lue::detail {

    auto normalize(std::string const& pathname) -> std::filesystem::path
    {
        return std::filesystem::canonical(pathname);
    }


    using CountByPath = std::map<std::filesystem::path, Count>;


    namespace {

        /*!
            @brief      .
            @tparam     .
            @param      .
            @return     The number of times this function is called for @a path. The value returned is always
                        larger than zero.

            The @a path passed in adds to this count, so the count returned is always larger than zero.

            This function must be called from the root locality, on the main thread.
        */
        auto count_by_path(CountByPath& count_by_path, std::filesystem::path const& path) -> Count
        {
            // If this is the first time path is added, initialize the count to 0
            count_by_path.try_emplace(path, 0);

            // Increment the count and return it
            return ++count_by_path.at(path);
        }


        auto from_lue_count_by_dataset_path() -> CountByPath&
        {
            static CountByPath count_by_path{};

            return count_by_path;
        }


        auto to_lue_count_by_dataset_path() -> CountByPath&
        {
            static CountByPath count_by_path{};

            return count_by_path;
        }

    }  // Anonymous namespace


    /*!
        @brief      Return the number of times the dataset pointed to by @a path is opened for reading
        @sa         count_by_path()

        This count can be used to order / serialize open_dataset calls. The count is incremented each time
        this function is called.
    */
    auto from_lue_order(std::filesystem::path const& path) -> Count
    {
        return count_by_path(from_lue_count_by_dataset_path(), path);
    }


    /*!
        @brief      Return the number of times the dataset pointed to by @a path is opened for writing
        @sa         count_by_path()

        This count can be used to order / serialize open_dataset calls. The count is incremented each time
        this function is called.
    */
    auto to_lue_order(std::filesystem::path const& path) -> Count
    {
        return count_by_path(to_lue_count_by_dataset_path(), path);
    }


    /*!
        @brief      Return the number of times the dataset pointed to by @a path has been opened for reading

        This count can be used to order / serialize open_dataset calls
    */
    auto current_from_lue_order(std::filesystem::path const& dataset_path) -> Count
    {
        auto const& count_by_path{from_lue_count_by_dataset_path()};

        return count_by_path.contains(dataset_path) ? count_by_path.at(dataset_path) : 0;
    }


    /*!
        @brief      Return the number of times the dataset pointed to by @a path has been opened for writing

        This count can be used to order / serialize open_dataset calls
    */
    auto current_to_lue_order(std::filesystem::path const& dataset_path) -> Count
    {
        auto const& count_by_path{to_lue_count_by_dataset_path()};

        return count_by_path.contains(dataset_path) ? count_by_path.at(dataset_path) : 0;
    }


#ifndef LUE_FRAMEWORK_WITH_PARALLEL_IO

    namespace {

        using CallFinished = std::map<std::filesystem::path, std::map<Count, hpx::shared_future<void>>>;


        /*!
            @brief      .
            @tparam     .
            @param      .
            @return     .
            @exception  .

            This function must be called from the root locality, on the main thread.
        */
        auto to_lue_finished() -> CallFinished&
        {
            static CallFinished to_lue_finished;

            return to_lue_finished;
        }


        /*!
            @brief      .
            @tparam     .
            @param      .
            @return     .
            @exception  .

            This function must be called from the root locality, on the main thread.
        */
        auto from_lue_finished() -> CallFinished&
        {
            static CallFinished from_lue_finished;

            return from_lue_finished;
        }


        /*!
            @brief      .
            @tparam     .
            @param      .
            @return     .
            @exception  .

            This function must be called from the root locality, on the main thread.
        */
        void add_call_finished(
            CallFinished& call_finished,
            std::filesystem::path const& path,
            Count const count,
            hpx::shared_future<void> future)
        {
#ifndef NDEBUG
            if (call_finished.contains(path))
            {
                lue_hpx_assert(!call_finished[path].contains(count));
            }
#endif
            lue_hpx_assert(future.valid());

            call_finished[path][count] = std::move(future);

            lue_hpx_assert(call_finished.contains(path));
            lue_hpx_assert(call_finished[path].contains(count));
            lue_hpx_assert(call_finished.at(path).at(count).valid());
        }


        /*!
            @brief      .
            @tparam     .
            @param      .
            @return     .
            @exception  .

            This function must be called from the root locality, on the main thread.
        */
        auto call_finished(CallFinished& call_finished, std::filesystem::path const& path, Count const count)
            -> hpx::shared_future<void>
        {
#ifndef NDEBUG
            if (count > 0)
            {
                lue_hpx_assert(call_finished.contains(path));
                lue_hpx_assert(call_finished[path].contains(count));
                lue_hpx_assert(call_finished[path][count].valid());
            }
#endif

            return count > 0 ? call_finished.at(path).at(count) : hpx::make_ready_future().share();
        }

    }  // Anonymous namespace


    /*!
        @brief      Add a @ future which becomes ready once call @a count to to_lue to @a path finishes

        This function must be called from the root locality, on the main thread.
    */
    void add_to_lue_finished(
        std::filesystem::path const& path, Count const count, hpx::shared_future<void> future)
    {
        add_call_finished(to_lue_finished(), path, count, std::move(future));
    }


    /*!
        @brief      Add a @ future which becomes ready once call @a count to from_lue from @a path finishes

        This function must be called from the root locality, on the main thread.
    */
    void add_from_lue_finished(
        std::filesystem::path const& path, Count const count, hpx::shared_future<void> future)
    {
        add_call_finished(from_lue_finished(), path, count, std::move(future));
    }


    /*!
        @brief      Return a future which becomes ready once call @a count to to_lue finishes

        This function must be called from the root locality, on the main thread.
    */
    auto to_lue_finished(std::filesystem::path const& path, Count const count) -> hpx::shared_future<void>
    {
        return call_finished(to_lue_finished(), path, count);
    }


    /*!
        @brief      Return a future which becomes ready once call @a count to from_lue from @a path finishes

        This function must be called from the root locality, on the main thread.
    */
    auto from_lue_finished(std::filesystem::path const& path, Count const count) -> hpx::shared_future<void>
    {
        return call_finished(from_lue_finished(), path, count);
    }

#endif


    using FileSerializer = Serializer<std::filesystem::path, Count>;


    namespace {

        auto to_lue_open_dataset_serializer() -> FileSerializer&
        {
            static Serializer<std::filesystem::path, Count> serializer{};

            return serializer;
        }


        auto to_lue_close_dataset_serializer() -> FileSerializer&
        {
            static Serializer<std::filesystem::path, Count> serializer{};

            return serializer;
        }


        auto from_lue_open_dataset_serializer() -> FileSerializer&
        {
            static Serializer<std::filesystem::path, Count> serializer{};

            return serializer;
        }


        auto from_lue_close_dataset_serializer() -> FileSerializer&
        {
            static Serializer<std::filesystem::path, Count> serializer{};

            return serializer;
        }

    }  // Anonymous namespace


    /*!
        @brief      Return a promise to set once to_lue has opened dataset @a path for call @a open_count

        This function must be called from a worker locality.
    */
    auto to_lue_open_dataset_promise_for(std::filesystem::path const& path, Count const open_count)
        -> hpx::promise<void>
    {
        return to_lue_open_dataset_serializer().promise_for(path, open_count);
    }


    /*!
        @brief      Return a promise to set once to_lue has closed dataset @a path for call @a open_count

        This function must be called from a worker locality.
    */
    auto to_lue_close_dataset_promise_for(std::filesystem::path const& path, Count const open_count)
        -> hpx::promise<void>
    {
        return to_lue_close_dataset_serializer().promise_for(path, open_count);
    }


    /*!
        @brief      Return a promise to set once from_lue has opened dataset @a path for call @a open_count

        This function must be called from a worker locality.
    */
    auto from_lue_open_dataset_promise_for(std::filesystem::path const& path, Count const open_count)
        -> hpx::promise<void>
    {
        return from_lue_open_dataset_serializer().promise_for(path, open_count);
    }


    /*!
        @brief      Return a promise to set once from_lue has opened dataset @a path for call @a open_count

        This function must be called from a worker locality.
    */
    auto from_lue_close_dataset_promise_for(std::filesystem::path const& path, Count const open_count)
        -> hpx::promise<void>
    {
        return from_lue_close_dataset_serializer().promise_for(path, open_count);
    }


    auto to_lue_open_dataset_when_predecessor_done(std::filesystem::path const& path, Count const open_count)
        -> hpx::shared_future<void>
    {
        // TODO: remove if not used
        return to_lue_open_dataset_serializer().when_predecessor_done(path, open_count);
    }


    /*!
        @brief      Return a future which will become ready once to_lue has closed dataset @a path for
                    call @a open_count - 1

        This function must be called from a worker locality.
    */
    auto to_lue_close_dataset_when_predecessor_done(std::filesystem::path const& path, Count const open_count)
        -> hpx::shared_future<void>
    {
        return to_lue_close_dataset_serializer().when_predecessor_done(path, open_count);
    }


    auto from_lue_open_dataset_when_predecessor_done(
        std::filesystem::path const& path, Count const open_count) -> hpx::shared_future<void>
    {
        // TODO: remove if not used
        return from_lue_open_dataset_serializer().when_predecessor_done(path, open_count);
    }


    /*!
        @brief      Return a future which will become ready once from_lue has closed dataset @a path for
                    call @a open_count - 1

        This function must be called from a worker locality.
    */
    auto from_lue_close_dataset_when_predecessor_done(
        std::filesystem::path const& path, Count const open_count) -> hpx::shared_future<void>
    {
        return from_lue_close_dataset_serializer().when_predecessor_done(path, open_count);
    }


    /*!
        @brief      Return whether a future is available which will become ready once from_lue has closed
                    dataset @a path for call @a count

        This function must be called from a worker locality.
    */
    auto from_lue_close_dataset_done_available(std::filesystem::path const& path, Count count) -> bool
    {
        lue_hpx_assert(count > 0);

        return from_lue_close_dataset_serializer().contains(path, count);
    }


    /*!
        @brief      Return a future which will become ready once from_lue has closed dataset @a path for call
                    @a count

        This function must be called from a worker locality.
    */
    auto from_lue_close_dataset_done(std::filesystem::path const& path, Count const count)
        -> hpx::shared_future<void>
    {
        return count > 0 ? from_lue_close_dataset_serializer().when_done(path, count)
                         : hpx::make_ready_future().share();
    }


    /*!
        @brief      Return whether a future is available which will become ready once to_lue has closed
                    dataset @a path for call @a count

        This function must be called from a worker locality.
    */
    auto to_lue_close_dataset_done_available(std::filesystem::path const& path, Count count) -> bool
    {
        lue_hpx_assert(count > 0);

        return to_lue_close_dataset_serializer().contains(path, count);
    }


    /*!
        @brief      Return a future which will become ready once to_lue has closed dataset @a path for call
                    @a count

        This function must be called from a worker locality.
    */
    auto to_lue_close_dataset_done(std::filesystem::path const& path, Count const count)
        -> hpx::shared_future<void>
    {
        return count > 0 ? to_lue_close_dataset_serializer().when_done(path, count)
                         : hpx::make_ready_future().share();
    }

}  // namespace lue::detail
