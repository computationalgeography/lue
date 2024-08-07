#define BOOST_TEST_MODULE lue framework core linear_curve
#include "lue/framework/core/linear_curve.hpp"
#include "lue/framework/test/stream.hpp"
#include <hpx/config.hpp>
#include <boost/test/included/unit_test.hpp>
#include <tuple>
#include <vector>


namespace {

    template<typename Shape, typename Coordinates>
    void fill_and_test(Shape const& shape, Coordinates const& result_we_want)
    {
        constexpr lue::Rank rank{lue::rank<Shape>};

        Coordinates result_we_got(lue::nr_elements(shape));

        lue::linear_curve<rank>(
            shape,

            [&result_we_got](lue::Index const idx, auto... idxs)
            { result_we_got[idx] = std::make_tuple(idxs...); }

        );

        BOOST_CHECK_EQUAL_COLLECTIONS(
            result_we_got.begin(), result_we_got.end(), result_we_want.begin(), result_we_want.end());
    }

}  // Anonymous namespace


BOOST_AUTO_TEST_CASE(d1)
{
    constexpr lue::Rank rank = 1;
    using Shape = lue::Shape<lue::Count, rank>;
    using Coordinate = std::tuple<lue::Index>;
    using Coordinates = std::vector<Coordinate>;

    {
        Shape const shape{4};
        Coordinates const result_we_want{
            {0},
            {1},
            {2},
            {3},
        };

        fill_and_test(shape, result_we_want);
    }

    {
        Shape const shape{1};
        Coordinates const result_we_want{
            {0},
        };

        fill_and_test(shape, result_we_want);
    }

    {
        Shape const shape{0};
        Coordinates const result_we_want{};
        fill_and_test(shape, result_we_want);
    }
}


BOOST_AUTO_TEST_CASE(d2)
{
    constexpr lue::Rank rank = 2;
    using Shape = lue::Shape<lue::Count, rank>;
    using Coordinate = std::tuple<lue::Index, lue::Index>;
    using Coordinates = std::vector<Coordinate>;

    {
        Shape const shape{4, 3};
        Coordinates const result_we_want{
            {0, 0},
            {0, 1},
            {0, 2},
            {1, 0},
            {1, 1},
            {1, 2},
            {2, 0},
            {2, 1},
            {2, 2},
            {3, 0},
            {3, 1},
            {3, 2},
        };

        fill_and_test(shape, result_we_want);
    }

    {
        Shape const shape{1, 1};
        Coordinates const result_we_want{
            {0, 0},
        };

        fill_and_test(shape, result_we_want);
    }

    {
        Shape const shape{0, 0};
        Coordinates const result_we_want{};
        fill_and_test(shape, result_we_want);
    }

    {
        Shape const shape{0, 1};
        Coordinates const result_we_want{};
        fill_and_test(shape, result_we_want);
    }

    {
        Shape const shape{1, 0};
        Coordinates const result_we_want{};
        fill_and_test(shape, result_we_want);
    }
}


BOOST_AUTO_TEST_CASE(d3)
{
    constexpr lue::Rank rank = 3;
    using Shape = lue::Shape<lue::Count, rank>;
    using Coordinate = std::tuple<lue::Index, lue::Index, lue::Index>;
    using Coordinates = std::vector<Coordinate>;

    {
        Shape const shape{2, 4, 3};
        Coordinates const result_we_want{
            {0, 0, 0}, {0, 0, 1}, {0, 0, 2}, {0, 1, 0}, {0, 1, 1}, {0, 1, 2}, {0, 2, 0}, {0, 2, 1},
            {0, 2, 2}, {0, 3, 0}, {0, 3, 1}, {0, 3, 2}, {1, 0, 0}, {1, 0, 1}, {1, 0, 2}, {1, 1, 0},
            {1, 1, 1}, {1, 1, 2}, {1, 2, 0}, {1, 2, 1}, {1, 2, 2}, {1, 3, 0}, {1, 3, 1}, {1, 3, 2},
        };

        fill_and_test(shape, result_we_want);
    }

    {
        Shape const shape{1, 1, 1};
        Coordinates const result_we_want{
            {0, 0, 0},
        };

        fill_and_test(shape, result_we_want);
    }

    {
        Shape const shape{0, 0, 0};
        Coordinates const result_we_want{};
        fill_and_test(shape, result_we_want);
    }

    {
        Shape const shape{0, 0, 1};
        Coordinates const result_we_want{};
        fill_and_test(shape, result_we_want);
    }

    {
        Shape const shape{0, 1, 0};
        Coordinates const result_we_want{};
        fill_and_test(shape, result_we_want);
    }

    {
        Shape const shape{1, 0, 0};
        Coordinates const result_we_want{};
        fill_and_test(shape, result_we_want);
    }
}
