#define BOOST_TEST_MODULE lue framework algorithm communicator_array
#include "flow_accumulation.hpp"
#include "lue/framework/algorithm/detail/communicator.hpp"
#include "lue/framework/algorithm/detail/communicator_array.hpp"
#include "lue/framework/test/array.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"


HPX_REGISTER_CHANNEL(int);


BOOST_AUTO_TEST_CASE(use_case_01)
{
    auto array = lue::test::spiral_in();

    lue::Rank const rank{2};
    lue::detail::CommunicatorArray<lue::detail::Communicator<int, rank>, rank> communicator_array{
        "communicator_array", array.localities()};

    // Send values around
    communicator_array(0, 0).send(lue::detail::accu::Direction::east, 1);
    communicator_array(0, 1).send(lue::detail::accu::Direction::east, 2);
    communicator_array(0, 2).send(lue::detail::accu::Direction::south, 3);
    communicator_array(1, 2).send(lue::detail::accu::Direction::south, 4);
    communicator_array(2, 2).send(lue::detail::accu::Direction::west, 5);
    communicator_array(2, 1).send(lue::detail::accu::Direction::west, 6);
    communicator_array(2, 0).send(lue::detail::accu::Direction::north, 7);
    communicator_array(1, 0).send(lue::detail::accu::Direction::north, 8);
    communicator_array(1, 0).send(lue::detail::accu::Direction::east, 9);

    // Receive values just sent
    BOOST_CHECK_EQUAL(communicator_array(0, 1).get(lue::detail::accu::Direction::west).get(), 1);
    BOOST_CHECK_EQUAL(communicator_array(0, 2).get(lue::detail::accu::Direction::west).get(), 2);
    BOOST_CHECK_EQUAL(communicator_array(1, 2).get(lue::detail::accu::Direction::north).get(), 3);
    BOOST_CHECK_EQUAL(communicator_array(2, 2).get(lue::detail::accu::Direction::north).get(), 4);
    BOOST_CHECK_EQUAL(communicator_array(2, 1).get(lue::detail::accu::Direction::east).get(), 5);
    BOOST_CHECK_EQUAL(communicator_array(2, 0).get(lue::detail::accu::Direction::east).get(), 6);
    BOOST_CHECK_EQUAL(communicator_array(1, 0).get(lue::detail::accu::Direction::south).get(), 7);
    BOOST_CHECK_EQUAL(communicator_array(0, 0).get(lue::detail::accu::Direction::south).get(), 8);
    BOOST_CHECK_EQUAL(communicator_array(1, 1).get(lue::detail::accu::Direction::west).get(), 9);
}
