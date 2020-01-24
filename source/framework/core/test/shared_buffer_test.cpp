#define BOOST_TEST_MODULE lue framework core shared_buffer
#include <boost/test/unit_test.hpp>
#include "lue/framework/core/shared_buffer.hpp"

namespace {

using Element = double;
using SharedBuffer = lue::SharedBuffer<Element>;
std::size_t const size = 10;

}  // Anonymous namespace


BOOST_AUTO_TEST_CASE(default_construct)
{
    SharedBuffer buffer{};
    BOOST_CHECK_EQUAL(buffer.size(), 0);
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

    // Deep copy
    {
        SharedBuffer buffer{other, lue::CopyMode::copy};

        BOOST_CHECK_EQUAL(buffer.size(), size);

        BOOST_CHECK_EQUAL(other.size(), size);

        // It's a shared buffer, but created as a deep copy from another one
        BOOST_CHECK(buffer.data() != other.data());
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
