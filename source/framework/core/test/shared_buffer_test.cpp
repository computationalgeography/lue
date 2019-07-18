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


BOOST_AUTO_TEST_CASE(construct_copy)
{
    auto source_buffer = std::make_unique<Element[]>(size);
    SharedBuffer buffer{source_buffer.get(), size, SharedBuffer::Mode::copy};
    BOOST_CHECK_EQUAL(buffer.size(), size);
    BOOST_CHECK(buffer.data() != source_buffer.get());

    // SharedBuffer must copy the elements from the source buffer. This must not
    // crash because of double delete...
}


BOOST_AUTO_TEST_CASE(construct_use)
{
    std::size_t const size = 10;
    auto source_buffer = std::make_unique<Element[]>(size);
    SharedBuffer buffer{source_buffer.get(), size, SharedBuffer::Mode::use};
    BOOST_CHECK_EQUAL(buffer.size(), size);
    BOOST_CHECK(buffer.data() == source_buffer.get());

    // SharedBuffer must not delete the source buffer. This must not crash
    // because of double delete...
}


BOOST_AUTO_TEST_CASE(construct_own)
{
    auto source_buffer = std::make_unique<Element[]>(size);
    Element* source_pointer = source_buffer.release();
    SharedBuffer buffer{source_pointer, size, SharedBuffer::Mode::own};
    BOOST_CHECK_EQUAL(buffer.size(), size);
    BOOST_CHECK(buffer.data() == source_pointer);

    // SharedBuffer must delete the source buffer. This must not leak...
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
