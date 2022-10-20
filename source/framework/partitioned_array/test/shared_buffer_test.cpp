#define BOOST_TEST_MODULE lue framework partitioned_array shared_buffer
#include <hpx/config.hpp>
#include <boost/test/unit_test.hpp>
#include "lue/framework/partitioned_array/shared_buffer.hpp"
#include <numeric>


namespace {

    using Element = double;
    using SharedBuffer = lue::SharedBuffer<Element>;
    std::size_t const size = 10;

}  // Anonymous namespace


BOOST_AUTO_TEST_CASE(default_construct)
{
    SharedBuffer buffer{};
    BOOST_CHECK_EQUAL(buffer.size(), 0);
    BOOST_CHECK_EQUAL(std::distance(buffer.begin(), buffer.end()), 0);
}


BOOST_AUTO_TEST_CASE(construct_new)
{
    SharedBuffer buffer{size};
    BOOST_CHECK_EQUAL(buffer.size(), size);
    BOOST_CHECK_EQUAL(std::distance(buffer.begin(), buffer.end()), size);
}


BOOST_AUTO_TEST_CASE(resize)
{
    std::size_t const new_size = 15;
    SharedBuffer buffer{size};
    buffer.resize(new_size);
    BOOST_CHECK_EQUAL(buffer.size(), new_size);
    BOOST_CHECK_EQUAL(std::distance(buffer.begin(), buffer.end()), new_size);
}


BOOST_AUTO_TEST_CASE(copy_construct)
{
    SharedBuffer other{size};

    // Shallow copy (default)
    {
        SharedBuffer buffer{other};

        BOOST_CHECK_EQUAL(buffer.size(), size);

        BOOST_CHECK_EQUAL(other.size(), size);

        // It's a shared buffer
        BOOST_CHECK(buffer.data() == other.data());
    }
}


BOOST_AUTO_TEST_CASE(copy_assign)
{
    SharedBuffer other{size};
    SharedBuffer buffer{};
    buffer = other;

    BOOST_CHECK_EQUAL(buffer.size(), size);

    BOOST_CHECK_EQUAL(other.size(), size);

    // It's a shared buffer
    BOOST_CHECK(buffer.data() == other.data());
}


BOOST_AUTO_TEST_CASE(move_construct)
{
    SharedBuffer other{size};
    SharedBuffer buffer{std::move(other)};

    BOOST_CHECK_EQUAL(buffer.size(), size);

    BOOST_CHECK(buffer.data() != other.data());
    BOOST_CHECK(other.data() == nullptr);
}


BOOST_AUTO_TEST_CASE(move_assign)
{
    SharedBuffer other{size};
    SharedBuffer buffer{};
    buffer = std::move(other);

    BOOST_CHECK_EQUAL(buffer.size(), size);

    BOOST_CHECK(buffer.data() != other.data());
    BOOST_CHECK(other.data() == nullptr);
}


BOOST_AUTO_TEST_CASE(erase)
{
    // +----+----+----+----+----+----+----+----+----+----+
    // | 10 | 11 | 12 | 13 | 14 | 15 | 16 | 17 | 18 | 19 |
    // +----+----+----+----+----+----+----+----+----+----+
    SharedBuffer buffer{10};
    std::iota(buffer.begin(), buffer.end(), 10);
    BOOST_CHECK_EQUAL(buffer.size(), 10);
    BOOST_CHECK_EQUAL(std::distance(buffer.begin(), buffer.end()), 10);
    BOOST_CHECK_EQUAL(buffer[0], 10);
    BOOST_CHECK_EQUAL(buffer[9], 19);

    // +----  +----+----+----+----+----+----+----+----+----+
    // | 10   | 11 | 12 | 13 | 14 | 15 | 16 | 17 | 18 | 19 |
    // +----  +----+----+----+----+----+----+----+----+----+
    buffer.erase(buffer.begin() + 0, buffer.begin() + 1);
    BOOST_CHECK_EQUAL(buffer.size(), 9);
    BOOST_CHECK_EQUAL(std::distance(buffer.begin(), buffer.end()), 9);
    BOOST_CHECK_EQUAL(buffer[0], 11);

    // +----+----+----+----+----+----+----+----+  ----+
    // | 11 | 12 | 13 | 14 | 15 | 16 | 17 | 18 |   19 |
    // +----+----+----+----+----+----+----+----+  ----+
    buffer.erase(buffer.begin() + 8, buffer.begin() + 9);
    BOOST_CHECK_EQUAL(buffer.size(), 8);
    BOOST_CHECK_EQUAL(std::distance(buffer.begin(), buffer.end()), 8);
    BOOST_CHECK_EQUAL(buffer[7], 18);

    // +----+----+----+  ----+----  +----+----+----+
    // | 11 | 12 | 13 |   14 | 15   | 16 | 17 | 18 |
    // +----+----+----+  ----+----  +----+----+----+
    buffer.erase(buffer.begin() + 3, buffer.begin() + 5);
    BOOST_CHECK_EQUAL(buffer.size(), 6);
    BOOST_CHECK_EQUAL(std::distance(buffer.begin(), buffer.end()), 6);
    BOOST_CHECK_EQUAL(buffer[2], 13);
    BOOST_CHECK_EQUAL(buffer[3], 16);
}
