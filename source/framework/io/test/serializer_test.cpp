#define BOOST_TEST_MODULE lue framework io serializer
#include "lue/framework/io/serializer.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"
#include <hpx/algorithm.hpp>
#include <random>


BOOST_AUTO_TEST_CASE(variable_raster)
{
    // Create a number of tasks that need to be run in the order of their task ID, not the order in which they
    // are created or some other order.

    using Key = std::string;
    using TaskID = std::int32_t;

    lue::Serializer<Key, TaskID> task_serializer{};

    Key const key = "my_tasks";
    TaskID const min_task_id = 1;
    TaskID const max_task_id = 100;

    std::random_device random_device{};
    std::mt19937 random_number_engine{random_device()};

    // Create collection of randomly ordered task IDs
    std::vector<TaskID> input_task_ids(max_task_id);
    std::ranges::iota(input_task_ids, min_task_id);
    std::ranges::shuffle(input_task_ids, random_number_engine);

    // Each task writes its ID into this collection. If all goes well, all task IDs should end up here,
    // ordered from min_task_id to max_task_id.
    std::vector<TaskID> output_task_ids{};

    // Collection of futures to wait on before we can start testing
    std::vector<hpx::future<void>> futures{};

    for (TaskID task_id : input_task_ids)
    {
        // This is how we can mark that we are done accessing some resource
        hpx::promise<void> promise = task_serializer.promise_for(key, task_id);

        // Create a task that will run after the one with task ID equal to task_id - 1 has finished
        futures.push_back(task_serializer.when_predecessor_done(key, task_id)
                              .then(
                                  [&output_task_ids, task_id, promise = std::move(promise)](
                                      hpx::future<void> const& future) mutable -> auto
                                  {
                                      BOOST_REQUIRE(future.valid());
                                      BOOST_REQUIRE(future.is_ready());

                                      // All threads access this same collection, but since these calls are
                                      // serialized, these accesses won't happen at the same time
                                      output_task_ids.push_back(task_id);

                                      // We are done, next in line can do its thing
                                      promise.set_value();
                                  }));
    }

    hpx::wait_all(futures.begin(), futures.end());

    BOOST_REQUIRE(!output_task_ids.empty());
    BOOST_CHECK_EQUAL(output_task_ids.size(), max_task_id - min_task_id + 1);
    BOOST_CHECK(std::ranges::is_sorted(output_task_ids));
    BOOST_CHECK(std::ranges::adjacent_find(output_task_ids) == output_task_ids.end());
    BOOST_CHECK_EQUAL(output_task_ids.front(), 1);
    BOOST_CHECK_EQUAL(output_task_ids.back(), max_task_id);
}
