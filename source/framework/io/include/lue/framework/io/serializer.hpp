#pragma once
#include "lue/framework/core/assert.hpp"
#include <hpx/future.hpp>
#include <boost/container/flat_map.hpp>
#include <concepts>


namespace lue {

    /*!
        @brief      Class for maintaining information that can be used to serialize concurrent tasks
        @tparam     Key Type for objects to group information by. In case of serializing access to a file,
                    this could be the type used to represent the name of the file, for example.
        @tparam     Generation Type to represent the generation / order / count
        @warning    This class is not thread-safe: call its member functions from a single thread
        @warning    There is no facility yet to allow instances of this class to shrink again

        An example use-case for this class is performing parallel I/O to an HDF5 file. When opening the same
        file multiple times, the collective open calls must be serialized. An instance of this class can be
        used to achieve this:

        - On the root locality, each call to the function that will result in the call to H5Open is associated
          with a count, starting with 1
        - All tasks that will result in the call to H5Open on the localities are passed in the count. Note
          that a task for a higher count can potentially be scheduled to run before a task for a lower count.
          This is the problem that needs to be prevented.
        - As long as an open call associated with a count before the current one has not finished yet, a task
          must not try to open this file. This can be achieved by attaching a continuation to the future
          associated with the open call that must finish first.

        In code:

        @code{.cpp}
        Serializer<std::string, Count> open_file_serializer{};

        void my_task(std::string const& pathname, Count const count)
        {
            // The promise is related to us / the current count
            hpx::promise<void> promise = open_file_serializer.promise_for(pathname, count);

            // The future is related to the one before us, with count count - 1
            hpx::future<void> predecessor_future = open_file_serializer.when_predecessor_done(
                pathname, count);

            hpx::future<Dataset> a_future = predecessor_future.then(
                [](hpx::future<void> const& future)
                {
                    // Call H5Open
                    // ...

                    // This will allow the next in line to call H5Open
                    promise.set_value();

                    // Return open dataset?
                    // ...
                }
            );

            // a_future will become ready once the call to H5Open has finished
        }
        @endcode
    */
    template<std::equality_comparable Key, std::totally_ordered Generation>
    class Serializer
    {

        public:

            /*!
                @brief      Request a promise associated by a future for the @a key and @a generation passed
                            in
                @warning    generation must be larger than zero

                The promise returned is related to the future which is related to the @a generation passed in.
                It can only be obtained once. Calling this function multiple times for the same generation
                will result in promises that are in a valid but unspecified state (they are useless).

                It is fine if this function is called for future generations first. That is the point of this
                class. It allows to serialize code in a context where calls can not easily be serialized.

                The caller is responsible for setting the value of the promise (call set_value()
                on it). Otherwise none of the tasks associated with a higher generation will ever be
                scheduled.
            */
            auto promise_for([[maybe_unused]] Key const& key, [[maybe_unused]] Generation const generation)
                -> hpx::promise<void>
            {
                lue_hpx_assert(generation > 0);

                // Map will be created if not present already
                auto& map{_tuples[key]};

                if (!map.contains(generation))
                {
                    // Function to add (promise, future) tuples by generation
                    auto add_tuple = [&map](Generation const generation) -> void
                    {
                        hpx::promise<void> promise{};
                        hpx::future<void> future{promise.get_future()};
                        map[generation] = std::make_tuple(std::move(promise), std::move(future));
                    };

                    // If the map is empty, we add a first (promise, future) tuple for a first generation (0).
                    // The promise's value is set immediately so the future is already ready. This way, we can
                    // return a future in when_predecessor_done for the first generation (1).
                    if (map.empty())
                    {
                        add_tuple(0);
                        hpx::promise<void>& promise = std::get<0>(map[0]);
                        lue_hpx_assert(!std::get<1>(map[0]).is_ready());
                        promise.set_value();
                        lue_hpx_assert(std::get<1>(map[0]).is_ready());
                    }

                    // Add (promise, future) tuples for the current generation passed in and any generations
                    // missing between the last one added until the current one
                    for (Generation new_order = map.nth(map.size() - 1)->first + 1; new_order <= generation;
                         ++new_order)
                    {
                        add_tuple(new_order);
                    }
                }

                lue_hpx_assert(map.contains(generation));
                lue_hpx_assert(map.size() > 1);

                hpx::promise<void>& promise = std::get<0>(map[generation]);

                return std::move(promise);
            }


            /*!
                @brief      Return the future associated with the **predecessor** call for the @a key and
                            @a generation passed in

                Attach a continuation to the future returned to serialize access to some resource.

                This function can only be called once for a @a key and @a generation. The future returned is
                the only one. Subsequent calls will return a future that is in a valid but unspecified state
                (it is useless).
            */
            auto when_predecessor_done(
                [[maybe_unused]] Key const& key, [[maybe_unused]] Generation const generation)
                -> hpx::future<void>
            {
                lue_hpx_assert(_tuples.contains(key));
                auto& map{_tuples[key]};
                lue_hpx_assert(map.contains(generation));
                lue_hpx_assert(map.size() > 1);

                hpx::future<void>& future = std::get<1>(map[generation - 1]);
                lue_hpx_assert(future.valid());

                return std::move(future);
            }

        private:

            using Promise = hpx::promise<void>;

            using Future = hpx::future<void>;

            using FutureTuple = std::tuple<Promise, Future>;

            using TupleByGeneration = boost::container::flat_map<Generation, FutureTuple>;

            using TupleByGenerationByKey = std::map<Key, TupleByGeneration>;

            TupleByGenerationByKey _tuples;
    };

}  // namespace lue
