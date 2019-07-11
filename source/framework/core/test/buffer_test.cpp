#define BOOST_TEST_MODULE lue framework core buffer
#include <boost/test/unit_test.hpp>
#include "lue/framework/core/buffer.hpp"

namespace {

using Element = double;
using Buffer = lue::Buffer<Element>;
std::size_t const size = 10;

}  // Anonymous namespace


BOOST_AUTO_TEST_CASE(default_construct)
{
    Buffer buffer{};
    BOOST_CHECK_EQUAL(buffer.size(), 0);
}


BOOST_AUTO_TEST_CASE(construct_new)
{
    Buffer buffer{size};
    BOOST_CHECK_EQUAL(buffer.size(), size);
    BOOST_CHECK_EQUAL(std::distance(buffer.begin(), buffer.end()), size);
}


BOOST_AUTO_TEST_CASE(construct_copy)
{
    auto source_buffer = std::make_unique<Element[]>(size);
    Buffer buffer{source_buffer.get(), size, Buffer::Mode::copy};
    BOOST_CHECK_EQUAL(buffer.size(), size);
    BOOST_CHECK(buffer.data() != source_buffer.get());

    // Buffer must copy the elements from the source buffer. This must not
    // crash because of double delete...
}


BOOST_AUTO_TEST_CASE(construct_use)
{
    std::size_t const size = 10;
    auto source_buffer = std::make_unique<Element[]>(size);
    Buffer buffer{source_buffer.get(), size, Buffer::Mode::use};
    BOOST_CHECK_EQUAL(buffer.size(), size);
    BOOST_CHECK(buffer.data() == source_buffer.get());

    // Buffer must not delete the source buffer. This must not crash
    // because of double delete...
}


BOOST_AUTO_TEST_CASE(construct_own)
{
    auto source_buffer = std::make_unique<Element[]>(size);
    Element* source_pointer = source_buffer.release();
    Buffer buffer{source_pointer, size, Buffer::Mode::own};
    BOOST_CHECK_EQUAL(buffer.size(), size);
    BOOST_CHECK(buffer.data() == source_pointer);

    // Buffer must delete the source buffer. This must not leak...
}


BOOST_AUTO_TEST_CASE(resize)
{
    std::size_t const new_size = 15;
    Buffer buffer{size};
    buffer.resize(new_size);
    BOOST_CHECK_EQUAL(buffer.size(), new_size);
}


BOOST_AUTO_TEST_CASE(copy_construct)
{
    Buffer other{size};
    Buffer buffer{other};

    BOOST_CHECK_EQUAL(buffer.size(), size);

    BOOST_CHECK_EQUAL(other.size(), size);

    BOOST_CHECK(buffer.data() != other.data());
}


BOOST_AUTO_TEST_CASE(copy_assign)
{
    Buffer other{size};
    Buffer buffer{};
    buffer = other;

    BOOST_CHECK_EQUAL(buffer.size(), size);

    BOOST_CHECK_EQUAL(other.size(), size);

    BOOST_CHECK(buffer.data() != other.data());
}


BOOST_AUTO_TEST_CASE(move_construct)
{
    Buffer other{size};
    Buffer buffer{std::move(other)};

    BOOST_CHECK_EQUAL(buffer.size(), size);

    BOOST_CHECK_EQUAL(other.size(), 0u);

    BOOST_CHECK(buffer.data() != other.data());
    BOOST_CHECK(other.data() == nullptr);
}


BOOST_AUTO_TEST_CASE(move_assign)
{
    Buffer other{size};
    Buffer buffer{};
    buffer = std::move(other);

    BOOST_CHECK_EQUAL(buffer.size(), size);

    BOOST_CHECK_EQUAL(other.size(), 0u);

    BOOST_CHECK(buffer.data() != other.data());
    BOOST_CHECK(other.data() == nullptr);
}
