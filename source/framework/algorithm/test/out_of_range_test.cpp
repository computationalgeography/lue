#define BOOST_TEST_MODULE lue framework algorithm out_of_range
#include "lue/framework/algorithm/out_of_range.hpp"
#include <boost/test/included/unit_test.hpp>


namespace {

    template<typename T>
    bool safe_add(T const& argument1, T const& argument2)
    {
        T result;
        return lue::safe_add<T>(argument1, argument2, result);
    }


    template<typename T>
    bool safe_multiply(T const& argument1, T const& argument2)
    {
        T result;
        return lue::safe_multiply<T>(argument1, argument2, result);
    }

}  // Anonymous namespace


BOOST_AUTO_TEST_CASE(safe_add_unsigned_integer)
{
    using Element = std::uint32_t;

    auto const max{std::numeric_limits<Element>::max()};

    // In
    BOOST_CHECK(safe_add<Element>(0, 0));
    BOOST_CHECK(safe_add<Element>(max, 0));
    BOOST_CHECK(safe_add<Element>(0, max));

    // Out
    BOOST_CHECK(!safe_add<Element>(max, 1));
    BOOST_CHECK(!safe_add<Element>(1, max));
}


BOOST_AUTO_TEST_CASE(safe_add_signed_integer)
{
    using Element = std::int32_t;

    auto const min{std::numeric_limits<Element>::min()};
    auto const max{std::numeric_limits<Element>::max()};

    // In
    BOOST_CHECK(safe_add<Element>(min, 0));
    BOOST_CHECK(safe_add<Element>(0, min));
    BOOST_CHECK(safe_add<Element>(max, 0));
    BOOST_CHECK(safe_add<Element>(0, max));

    // Out
    BOOST_CHECK(!safe_add<Element>(min, -1));
    BOOST_CHECK(!safe_add<Element>(-1, min));
    BOOST_CHECK(!safe_add<Element>(max, 1));
    BOOST_CHECK(!safe_add<Element>(1, max));
}


BOOST_AUTO_TEST_CASE(safe_add_floating_point)
{
    using Element = float;

    auto const smallest{std::numeric_limits<Element>::min()};
    auto const lowest{std::numeric_limits<Element>::lowest()};
    auto const highest{std::numeric_limits<Element>::max()};
    // auto const epsilon{std::numeric_limits<Element>::epsilon()};

    // In
    BOOST_CHECK(safe_add<Element>(smallest, 0));
    BOOST_CHECK(safe_add<Element>(0, smallest));
    BOOST_CHECK(safe_add<Element>(lowest, 0));
    BOOST_CHECK(safe_add<Element>(0, lowest));
    BOOST_CHECK(safe_add<Element>(highest, 0));
    BOOST_CHECK(safe_add<Element>(0, highest));

    // Out
    // TODO
}


BOOST_AUTO_TEST_CASE(safe_multiply_unsigned_integer)
{
    using Element = std::uint32_t;

    auto const max{std::numeric_limits<Element>::max()};

    // In
    BOOST_CHECK(safe_multiply<Element>(0, 0));
    BOOST_CHECK(safe_multiply<Element>(max, 1));
    BOOST_CHECK(safe_multiply<Element>(1, max));

    // Out
    BOOST_CHECK(!safe_multiply<Element>(max, 2));
    BOOST_CHECK(!safe_multiply<Element>(2, max));
}


BOOST_AUTO_TEST_CASE(safe_multiply_signed_integer)
{
    using Element = std::int32_t;

    auto const min{std::numeric_limits<Element>::min()};
    auto const max{std::numeric_limits<Element>::max()};

    // In
    BOOST_CHECK(safe_multiply<Element>(0, 0));
    BOOST_CHECK(safe_multiply<Element>(min, 1));
    BOOST_CHECK(safe_multiply<Element>(1, min));
    BOOST_CHECK(safe_multiply<Element>(max, 1));
    BOOST_CHECK(safe_multiply<Element>(1, max));

    // Out
    BOOST_CHECK(!safe_multiply<Element>(min, 2));
    BOOST_CHECK(!safe_multiply<Element>(2, min));
    BOOST_CHECK(!safe_multiply<Element>(min, -2));
    BOOST_CHECK(!safe_multiply<Element>(-2, min));
    BOOST_CHECK(!safe_multiply<Element>(max, 2));
    BOOST_CHECK(!safe_multiply<Element>(2, max));
    BOOST_CHECK(!safe_multiply<Element>(max, -2));
    BOOST_CHECK(!safe_multiply<Element>(-2, max));
}


BOOST_AUTO_TEST_CASE(safe_multiply_floating_point)
{
    using Element = float;

    auto const smallest{std::numeric_limits<Element>::min()};
    auto const lowest{std::numeric_limits<Element>::lowest()};
    auto const highest{std::numeric_limits<Element>::max()};
    // auto const epsilon{std::numeric_limits<Element>::epsilon()};

    // In
    BOOST_CHECK(safe_multiply<Element>(smallest, 1));
    BOOST_CHECK(safe_multiply<Element>(1, smallest));
    BOOST_CHECK(safe_multiply<Element>(lowest, 1));
    BOOST_CHECK(safe_multiply<Element>(1, lowest));
    BOOST_CHECK(safe_multiply<Element>(highest, 1));
    BOOST_CHECK(safe_multiply<Element>(1, highest));

    // Out
    // TODO
}


namespace {

    template<typename T>
    bool add_within_range(T const& argument1, T const& argument2)
    {
        return lue::add_within_range<T>(argument1, argument2, argument1 + argument2);
    }


    template<typename T>
    bool add_out_of_range(T const& argument1, T const& argument2)
    {
        return lue::add_out_of_range<T>(argument1, argument2, argument1 + argument2);
    }


    template<typename T>
    bool multiply_within_range(T const& argument1, T const& argument2)
    {
        return lue::multiply_within_range<T>(argument1, argument2, argument1 * argument2);
    }


    template<typename T>
    bool multiply_out_of_range(T const& argument1, T const& argument2)
    {
        return lue::multiply_out_of_range<T>(argument1, argument2, argument1 * argument2);
    }

}  // Anonymous namespace


BOOST_AUTO_TEST_CASE(add_unsigned_integer)
{
    using Element = std::uint32_t;

    auto const max{std::numeric_limits<Element>::max()};

    // In
    BOOST_CHECK(add_within_range<Element>(0, 0));
    BOOST_CHECK(add_within_range<Element>(max, 0));
    BOOST_CHECK(add_within_range<Element>(0, max));

    // Out
    BOOST_CHECK(add_out_of_range<Element>(max, 1));
    BOOST_CHECK(add_out_of_range<Element>(1, max));
}


BOOST_AUTO_TEST_CASE(add_signed_integer)
{
    using Element = std::int32_t;

    auto const min{std::numeric_limits<Element>::min()};
    auto const max{std::numeric_limits<Element>::max()};

    // In
    BOOST_CHECK(add_within_range<Element>(min, 0));
    BOOST_CHECK(add_within_range<Element>(0, min));
    BOOST_CHECK(add_within_range<Element>(max, 0));
    BOOST_CHECK(add_within_range<Element>(0, max));

    // Out
    BOOST_CHECK(add_out_of_range<Element>(min, -1));
    BOOST_CHECK(add_out_of_range<Element>(-1, min));
    BOOST_CHECK(add_out_of_range<Element>(max, 1));
    BOOST_CHECK(add_out_of_range<Element>(1, max));
}


BOOST_AUTO_TEST_CASE(add_floating_point)
{
    using Element = float;

    auto const smallest{std::numeric_limits<Element>::min()};
    auto const lowest{std::numeric_limits<Element>::lowest()};
    auto const highest{std::numeric_limits<Element>::max()};
    // auto const epsilon{std::numeric_limits<Element>::epsilon()};

    // In
    BOOST_CHECK(add_within_range<Element>(smallest, 0));
    BOOST_CHECK(add_within_range<Element>(0, smallest));
    BOOST_CHECK(add_within_range<Element>(lowest, 0));
    BOOST_CHECK(add_within_range<Element>(0, lowest));
    BOOST_CHECK(add_within_range<Element>(highest, 0));
    BOOST_CHECK(add_within_range<Element>(0, highest));

    // Out
    // TODO
}


BOOST_AUTO_TEST_CASE(multiply_unsigned_integer)
{
    using Element = std::uint32_t;

    auto const max{std::numeric_limits<Element>::max()};

    // In
    BOOST_CHECK(multiply_within_range<Element>(0, 0));
    BOOST_CHECK(multiply_within_range<Element>(max, 1));
    BOOST_CHECK(multiply_within_range<Element>(1, max));

    // Out
    BOOST_CHECK(multiply_out_of_range<Element>(max, 2));
    BOOST_CHECK(multiply_out_of_range<Element>(2, max));
}


BOOST_AUTO_TEST_CASE(multiply_signed_integer)
{
    using Element = std::int32_t;

    auto const min{std::numeric_limits<Element>::min()};
    auto const max{std::numeric_limits<Element>::max()};

    // In
    BOOST_CHECK(multiply_within_range<Element>(0, 0));
    BOOST_CHECK(multiply_within_range<Element>(min, 1));
    BOOST_CHECK(multiply_within_range<Element>(1, min));
    BOOST_CHECK(multiply_within_range<Element>(max, 1));
    BOOST_CHECK(multiply_within_range<Element>(1, max));

    // Out
    BOOST_CHECK(multiply_out_of_range<Element>(min, 2));
    BOOST_CHECK(multiply_out_of_range<Element>(2, min));
    BOOST_CHECK(multiply_out_of_range<Element>(min, -2));
    BOOST_CHECK(multiply_out_of_range<Element>(-2, min));
    BOOST_CHECK(multiply_out_of_range<Element>(max, 2));
    BOOST_CHECK(multiply_out_of_range<Element>(2, max));
    BOOST_CHECK(multiply_out_of_range<Element>(max, -2));
    BOOST_CHECK(multiply_out_of_range<Element>(-2, max));
}


BOOST_AUTO_TEST_CASE(multiply_floating_point)
{
    using Element = float;

    auto const smallest{std::numeric_limits<Element>::min()};
    auto const lowest{std::numeric_limits<Element>::lowest()};
    auto const highest{std::numeric_limits<Element>::max()};
    // auto const epsilon{std::numeric_limits<Element>::epsilon()};

    // In
    BOOST_CHECK(multiply_within_range<Element>(smallest, 1));
    BOOST_CHECK(multiply_within_range<Element>(1, smallest));
    BOOST_CHECK(multiply_within_range<Element>(lowest, 1));
    BOOST_CHECK(multiply_within_range<Element>(1, lowest));
    BOOST_CHECK(multiply_within_range<Element>(highest, 1));
    BOOST_CHECK(multiply_within_range<Element>(1, highest));

    // Out
    // TODO
}
