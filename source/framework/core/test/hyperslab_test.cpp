#define BOOST_TEST_MODULE lue framework core hyperslab
#include "lue/framework/core/hyperslab.hpp"
#include "lue/stream.hpp"
#include <boost/test/included/unit_test.hpp>


BOOST_AUTO_TEST_CASE(construct_2d_odd_shape)
{
    lue::Rank const rank{2};
    using Hyperslab = lue::Hyperslab<rank>;

    Hyperslab::Center center{20, 30};
    Hyperslab::Shape shape{7, 5};
    Hyperslab hyperslab{center, shape};

    Hyperslab::Offsets const offsets{17, 28};
    Hyperslab::Counts const counts{shape};
    Hyperslab::Strides const strides{{1, 1}};

    BOOST_CHECK_EQUAL(hyperslab.offsets(), offsets);
    BOOST_CHECK_EQUAL(hyperslab.counts(), counts);
    BOOST_CHECK_EQUAL(hyperslab.strides(), strides);
}


BOOST_AUTO_TEST_CASE(construct_2d_even_shape)
{
    lue::Rank const rank{2};
    using Hyperslab = lue::Hyperslab<rank>;

    Hyperslab::Center center{20, 30};
    Hyperslab::Shape shape{6, 4};
    Hyperslab hyperslab{center, shape};

    Hyperslab::Offsets const offsets{17, 28};
    Hyperslab::Counts const counts{shape};
    Hyperslab::Strides const strides{{1, 1}};

    BOOST_CHECK_EQUAL(hyperslab.offsets(), offsets);
    BOOST_CHECK_EQUAL(hyperslab.counts(), counts);
    BOOST_CHECK_EQUAL(hyperslab.strides(), strides);
}


BOOST_AUTO_TEST_CASE(valid_hyperslab)
{
    lue::Rank const rank{2};
    using Hyperslab = lue::Hyperslab<rank>;

    {
        // Empty hyperslab
        Hyperslab::Center const center{30, 20};
        Hyperslab::Shape const shape{0, 0};
        BOOST_CHECK_NO_THROW(Hyperslab(center, shape));
    }

    {
        // Empty hyperslab
        Hyperslab::Center const center{30, 20};
        Hyperslab::Shape const shape{60, 0};
        BOOST_CHECK_NO_THROW(Hyperslab(center, shape));
    }

    {
        // Empty hyperslab
        Hyperslab::Center const center{30, 20};
        Hyperslab::Shape const shape{0, 40};
        BOOST_CHECK_NO_THROW(Hyperslab(center, shape));
    }

    {
        // Hyperslab corresponds with whole array
        Hyperslab::Center const center{30, 20};
        Hyperslab::Shape const shape{60, 40};
        BOOST_CHECK_NO_THROW(Hyperslab(center, shape));
    }
}


BOOST_AUTO_TEST_CASE(invalid_hyperslab)
{
    lue::Rank const rank{2};
    using Hyperslab = lue::Hyperslab<rank>;

    {
        Hyperslab::Center const center{30, 20};
        Hyperslab::Shape const shape{62, 42};

        BOOST_CHECK_EXCEPTION(
            Hyperslab(center, shape),
            std::runtime_error,
            [](auto const& exception)
            { return std::string(exception.what()).find("extents beyond the limits") != std::string::npos; });
    }

    {
        Hyperslab::Center const center{30, 20};
        Hyperslab::Shape const shape{62, 40};

        BOOST_CHECK_EXCEPTION(
            Hyperslab(center, shape),
            std::runtime_error,
            [](auto const& exception)
            { return std::string(exception.what()).find("extents beyond the limits") != std::string::npos; });
    }

    {
        Hyperslab::Center const center{30, 20};
        Hyperslab::Shape const shape{60, 42};

        BOOST_CHECK_EXCEPTION(
            Hyperslab(center, shape),
            std::runtime_error,
            [](auto const& exception)
            { return std::string(exception.what()).find("extents beyond the limits") != std::string::npos; });
    }
}


BOOST_AUTO_TEST_CASE(is_contained_by)
{
    lue::Rank const rank{2};
    using Hyperslab = lue::Hyperslab<rank>;

    Hyperslab::Center center{20, 30};
    Hyperslab::Shape shape{7, 5};
    Hyperslab hyperslab{center, shape};

    BOOST_CHECK(hyperslab.is_contained_by(Hyperslab::Shape{24, 33}));
    BOOST_CHECK(!hyperslab.is_contained_by(Hyperslab::Shape{23, 33}));
    BOOST_CHECK(!hyperslab.is_contained_by(Hyperslab::Shape{24, 32}));
}
