#define BOOST_TEST_MODULE lue framework core domain_decomposition
#include "lue/framework/core/domain_decomposition.hpp"
#include "lue/framework/test/compare.hpp"
#include "lue/framework/test/stream.hpp"
#include <hpx/config.hpp>
#include <boost/test/unit_test.hpp>


BOOST_AUTO_TEST_CASE(nr_2d_partitions)
{
    using Index = std::uint64_t;
    std::size_t const rank = 2;
    using Shape = lue::Shape<Index, rank>;

    Shape area_shape{6, 4};

    {
        Shape partition_shape{1, 1};
        auto nr_partitions = lue::nr_partitions(area_shape, partition_shape);
        BOOST_CHECK_EQUAL(nr_partitions, 24);
    }

    {
        Shape partition_shape{2, 2};
        auto nr_partitions = lue::nr_partitions(area_shape, partition_shape);
        BOOST_CHECK_EQUAL(nr_partitions, 6);
    }

    {
        Shape partition_shape{3, 3};
        auto nr_partitions = lue::nr_partitions(area_shape, partition_shape);
        BOOST_CHECK_EQUAL(nr_partitions, 4);
    }

    {
        Shape partition_shape{4, 4};
        auto nr_partitions = lue::nr_partitions(area_shape, partition_shape);
        BOOST_CHECK_EQUAL(nr_partitions, 2);
    }

    {
        Shape partition_shape{5, 5};
        auto nr_partitions = lue::nr_partitions(area_shape, partition_shape);
        BOOST_CHECK_EQUAL(nr_partitions, 2);
    }

    {
        Shape partition_shape{10, 10};
        auto nr_partitions = lue::nr_partitions(area_shape, partition_shape);
        BOOST_CHECK_EQUAL(nr_partitions, 1);
    }
}


BOOST_AUTO_TEST_CASE(linear_to_1d_shape_index)
{
    using Index = std::uint64_t;
    std::size_t const rank = 1;
    using Shape = lue::Shape<Index, rank>;
    using Indices = lue::Indices<Index, rank>;

    Shape area_shape{10};
    Index index;

    {
        index = 0;
        auto indices = lue::linear_to_shape_index(area_shape, index);
        BOOST_CHECK(indices == (Indices{0}));
    }

    {
        index = 5;
        auto indices = lue::linear_to_shape_index(area_shape, index);
        BOOST_CHECK(indices == (Indices{5}));
    }

    {
        index = 9;
        auto indices = lue::linear_to_shape_index(area_shape, index);
        BOOST_CHECK(indices == (Indices{9}));
    }
}


BOOST_AUTO_TEST_CASE(linear_to_2d_shape_index)
{
    using Index = std::uint64_t;
    std::size_t const rank = 2;
    using Shape = lue::Shape<Index, rank>;
    using Indices = lue::Indices<Index, rank>;

    Shape area_shape{6, 4};
    Index index;

    {
        index = 0;
        auto indices = lue::linear_to_shape_index(area_shape, index);
        BOOST_CHECK(indices == (Indices{0, 0}));
    }

    {
        index = 3;
        auto indices = lue::linear_to_shape_index(area_shape, index);
        BOOST_CHECK(indices == (Indices{0, 3}));
    }

    {
        index = 4;
        auto indices = lue::linear_to_shape_index(area_shape, index);
        BOOST_CHECK(indices == (Indices{1, 0}));
    }

    {
        index = 23;
        auto indices = lue::linear_to_shape_index(area_shape, index);
        BOOST_CHECK(indices == (Indices{5, 3}));
    }
}


BOOST_AUTO_TEST_CASE(linear_to_3d_shape_index)
{
    using Index = std::uint64_t;
    std::size_t const rank = 3;
    using Shape = lue::Shape<Index, rank>;
    using Indices = lue::Indices<Index, rank>;

    Shape area_shape{2, 6, 4};
    Index index;

    {
        index = 0;
        auto indices = lue::linear_to_shape_index(area_shape, index);
        BOOST_CHECK(indices == (Indices{0, 0, 0}));
    }

    {
        index = 47;
        auto indices = lue::linear_to_shape_index(area_shape, index);
        BOOST_CHECK(indices == (Indices{1, 5, 3}));
    }
}


BOOST_AUTO_TEST_CASE(clamp_area_shape)
{
    using Index = std::uint64_t;
    std::size_t const rank = 2;
    using Shape = lue::Shape<Index, rank>;

    Shape area_shape{6, 4};

    {
        Shape partition_shape{1, 1};
        auto shape = lue::clamp_area_shape(area_shape, partition_shape);
        BOOST_CHECK(shape == (Shape{6, 4}));
    }

    {
        Shape partition_shape{2, 2};
        auto shape = lue::clamp_area_shape(area_shape, partition_shape);
        BOOST_CHECK(shape == (Shape{6, 4}));
    }

    {
        Shape partition_shape{3, 3};
        auto shape = lue::clamp_area_shape(area_shape, partition_shape);
        BOOST_CHECK(shape == (Shape{6, 6}));
    }

    {
        Shape partition_shape{4, 4};
        auto shape = lue::clamp_area_shape(area_shape, partition_shape);
        BOOST_CHECK(shape == (Shape{8, 4}));
    }

    {
        Shape partition_shape{5, 5};
        auto shape = lue::clamp_area_shape(area_shape, partition_shape);
        BOOST_CHECK(shape == (Shape{10, 5}));
    }
}


BOOST_AUTO_TEST_CASE(shape_in_partitions)
{
    using Index = std::uint64_t;
    std::size_t const rank = 2;
    using Shape = lue::Shape<Index, rank>;

    Shape area_shape{6, 4};

    {
        Shape partition_shape{1, 1};
        auto shape = lue::shape_in_partitions(area_shape, partition_shape);
        BOOST_CHECK(shape == (Shape{6, 4}));
    }

    {
        Shape partition_shape{2, 2};
        auto shape = lue::shape_in_partitions(area_shape, partition_shape);
        BOOST_CHECK(shape == (Shape{3, 2}));
    }

    {
        Shape partition_shape{3, 3};
        auto shape = lue::shape_in_partitions(area_shape, partition_shape);
        BOOST_CHECK(shape == (Shape{2, 2}));
    }

    {
        Shape partition_shape{4, 4};
        auto shape = lue::shape_in_partitions(area_shape, partition_shape);
        BOOST_CHECK(shape == (Shape{2, 1}));
    }

    {
        Shape partition_shape{5, 5};
        auto shape = lue::shape_in_partitions(area_shape, partition_shape);
        BOOST_CHECK(shape == (Shape{2, 1}));
    }
}


BOOST_AUTO_TEST_CASE(max_partition_shape_1d)
{
    using Shape = lue::Shape<std::uint32_t, 1>;

    {
        Shape const shape{10};
        Shape const shape_we_got = lue::max_partition_shape(shape, 1u);
        Shape const shape_we_want = Shape{10};

        BOOST_CHECK_EQUAL(shape_we_got, shape_we_want);
    }

    {
        Shape const shape{10};
        Shape const shape_we_got = lue::max_partition_shape(shape, 2u);
        Shape const shape_we_want = Shape{5};

        BOOST_CHECK_EQUAL(shape_we_got, shape_we_want);
    }

    {
        Shape const shape{10};
        Shape const shape_we_got = lue::max_partition_shape(shape, 3u);
        Shape const shape_we_want = Shape{3};

        BOOST_CHECK_EQUAL(shape_we_got, shape_we_want);
    }

    {
        Shape const shape{10};
        Shape const shape_we_got = lue::max_partition_shape(shape, 4u);
        Shape const shape_we_want = Shape{2};

        BOOST_CHECK_EQUAL(shape_we_got, shape_we_want);
    }

    {
        Shape const shape{10};
        Shape const shape_we_got = lue::max_partition_shape(shape, 5u);
        Shape const shape_we_want = Shape{2};

        BOOST_CHECK_EQUAL(shape_we_got, shape_we_want);
    }

    {
        Shape const shape{10};
        Shape const shape_we_got = lue::max_partition_shape(shape, 6u);
        Shape const shape_we_want = Shape{1};

        BOOST_CHECK_EQUAL(shape_we_got, shape_we_want);
    }

    {
        Shape const shape{10};
        Shape const shape_we_got = lue::max_partition_shape(shape, 10u);
        Shape const shape_we_want = Shape{1};

        BOOST_CHECK_EQUAL(shape_we_got, shape_we_want);
    }
}


BOOST_AUTO_TEST_CASE(max_partition_shape_2d)
{
    using Shape = lue::Shape<std::uint32_t, 2>;

    {
        Shape const shape{10, 10};
        Shape const shape_we_got = lue::max_partition_shape(shape, 1u);
        Shape const shape_we_want = Shape{10, 10};

        BOOST_CHECK_EQUAL(shape_we_got, shape_we_want);
    }

    {
        Shape const shape{10, 10};
        Shape const shape_we_got = lue::max_partition_shape(shape, 2u);
        Shape const shape_we_want = Shape{5, 10};

        BOOST_CHECK_EQUAL(shape_we_got, shape_we_want);
    }

    {
        Shape const shape{10, 10};
        Shape const shape_we_got = lue::max_partition_shape(shape, 3u);
        Shape const shape_we_want = Shape{5, 5};

        BOOST_CHECK_EQUAL(shape_we_got, shape_we_want);
    }

    {
        Shape const shape{10, 10};
        Shape const shape_we_got = lue::max_partition_shape(shape, 4u);
        Shape const shape_we_want = Shape{5, 5};

        BOOST_CHECK_EQUAL(shape_we_got, shape_we_want);
    }

    {
        Shape const shape{10, 10};
        Shape const shape_we_got = lue::max_partition_shape(shape, 25u);
        Shape const shape_we_want = Shape{2, 2};

        BOOST_CHECK_EQUAL(shape_we_got, shape_we_want);
    }

    {
        Shape const shape{10, 10};
        Shape const shape_we_got = lue::max_partition_shape(shape, 100u);
        Shape const shape_we_want = Shape{1, 1};

        BOOST_CHECK_EQUAL(shape_we_got, shape_we_want);
    }
}


BOOST_AUTO_TEST_CASE(partition_shapes_1d)
{
    lue::Rank const rank{1};
    using Shape = lue::Shape<lue::Index, rank>;
    using Array = lue::Array<Shape, rank>;

    {
        // 30 / 10 → 3
        Shape const array_shape{30};
        Shape const partition_shape{10};
        Array partition_shapes = lue::partition_shapes(array_shape, partition_shape);

        BOOST_REQUIRE_EQUAL(partition_shapes.shape(), (Shape{3}));
        BOOST_CHECK_EQUAL(partition_shapes(0), partition_shape);
        BOOST_CHECK_EQUAL(partition_shapes(1), partition_shape);
        BOOST_CHECK_EQUAL(partition_shapes(2), partition_shape);
    }


    {
        // 30 / 8 → 3
        Shape const array_shape{30};
        Shape const partition_shape{8};
        Array partition_shapes = lue::partition_shapes(array_shape, partition_shape);

        BOOST_REQUIRE_EQUAL(partition_shapes.shape(), (Shape{3}));
        BOOST_CHECK_EQUAL(partition_shapes(0), partition_shape);
        BOOST_CHECK_EQUAL(partition_shapes(1), partition_shape);
        BOOST_CHECK_EQUAL(partition_shapes(2), Shape{14});
    }

    {
        // 30 / 0 → exception
        Shape const array_shape{30};
        Shape const partition_shape{0};

        BOOST_CHECK_THROW(lue::partition_shapes(array_shape, partition_shape), std::runtime_error);
    }

    {
        // 0 / 8 → exception
        Shape const array_shape{0};
        Shape const partition_shape{8};

        BOOST_CHECK_THROW(lue::partition_shapes(array_shape, partition_shape), std::runtime_error);
    }

    {
        // 8 / 30 → exception
        Shape const array_shape{8};
        Shape const partition_shape{30};

        BOOST_CHECK_THROW(lue::partition_shapes(array_shape, partition_shape), std::runtime_error);
    }
}


BOOST_AUTO_TEST_CASE(partition_shapes_2d)
{
    lue::Rank const rank{2};
    using Shape = lue::Shape<lue::Index, rank>;
    using Array = lue::Array<Shape, rank>;

    {
        // 30 x 20 / 10 x 5 → 3 x 4
        Shape const array_shape{30, 20};
        Shape const partition_shape{10, 5};
        Array partition_shapes = lue::partition_shapes(array_shape, partition_shape);

        BOOST_REQUIRE_EQUAL(partition_shapes.shape(), (Shape{3, 4}));
        BOOST_CHECK_EQUAL(partition_shapes(0, 0), partition_shape);
        BOOST_CHECK_EQUAL(partition_shapes(0, 1), partition_shape);
        BOOST_CHECK_EQUAL(partition_shapes(0, 2), partition_shape);
        BOOST_CHECK_EQUAL(partition_shapes(0, 3), partition_shape);

        BOOST_CHECK_EQUAL(partition_shapes(1, 0), partition_shape);
        BOOST_CHECK_EQUAL(partition_shapes(1, 1), partition_shape);
        BOOST_CHECK_EQUAL(partition_shapes(1, 2), partition_shape);
        BOOST_CHECK_EQUAL(partition_shapes(1, 3), partition_shape);

        BOOST_CHECK_EQUAL(partition_shapes(2, 0), partition_shape);
        BOOST_CHECK_EQUAL(partition_shapes(2, 1), partition_shape);
        BOOST_CHECK_EQUAL(partition_shapes(2, 2), partition_shape);
        BOOST_CHECK_EQUAL(partition_shapes(2, 3), partition_shape);
    }

    {
        // 30 x 20 / 8 x 6 → 3 x 3
        Shape const array_shape{30, 20};
        Shape const partition_shape{8, 6};
        Array partition_shapes = lue::partition_shapes(array_shape, partition_shape);

        BOOST_REQUIRE_EQUAL(partition_shapes.shape(), (Shape{3, 3}));

        BOOST_CHECK_EQUAL(partition_shapes(0, 0), partition_shape);
        BOOST_CHECK_EQUAL(partition_shapes(0, 1), partition_shape);
        BOOST_CHECK_EQUAL(partition_shapes(0, 2), (Shape{8, 8}));

        BOOST_CHECK_EQUAL(partition_shapes(1, 0), partition_shape);
        BOOST_CHECK_EQUAL(partition_shapes(1, 1), partition_shape);
        BOOST_CHECK_EQUAL(partition_shapes(1, 2), (Shape{8, 8}));

        BOOST_CHECK_EQUAL(partition_shapes(2, 0), (Shape{14, 6}));
        BOOST_CHECK_EQUAL(partition_shapes(2, 1), (Shape{14, 6}));
        BOOST_CHECK_EQUAL(partition_shapes(2, 2), (Shape{14, 8}));
    }

    {
        // 30 x 20 / 0 x 0 → exception
        Shape const array_shape{30, 20};
        Shape const partition_shape{0, 0};

        BOOST_CHECK_THROW(lue::partition_shapes(array_shape, partition_shape), std::runtime_error);
    }

    {
        // 0 x 0 / 8 x 6 → exception
        Shape const array_shape{0, 0};
        Shape const partition_shape{8, 6};

        BOOST_CHECK_THROW(lue::partition_shapes(array_shape, partition_shape), std::runtime_error);
    }

    {
        // 8 x 6 / 30 x 20 → exception
        Shape const array_shape{8, 6};
        Shape const partition_shape{30, 20};

        BOOST_CHECK_THROW(lue::partition_shapes(array_shape, partition_shape), std::runtime_error);
    }
}


BOOST_AUTO_TEST_CASE(default_partition_shape_1d)
{
    lue::Count const default_partition_size{2000 * 2000};
    lue::Rank const rank{1};
    using Shape = lue::Shape<lue::Index, rank>;
    using Count = lue::Count;

    // ----- Empty and small stuff -----
    {
        Count const nr_worker_threads{1};
        Shape const array_shape{0};
        Shape const shape_we_want{0};

        Shape const shape_we_got{lue::default_partition_shape(array_shape, nr_worker_threads)};

        BOOST_CHECK_EQUAL(shape_we_got, shape_we_want);
    }

    {
        Count const nr_worker_threads{1};
        Shape const array_shape{1};
        Shape const shape_we_want{1};

        Shape const shape_we_got{lue::default_partition_shape(array_shape, nr_worker_threads)};

        BOOST_CHECK_EQUAL(shape_we_got, shape_we_want);
    }

    {
        Count const nr_worker_threads{2};
        Shape const array_shape{1};
        Shape const shape_we_want{1};

        Shape const shape_we_got{lue::default_partition_shape(array_shape, nr_worker_threads)};

        BOOST_CHECK_EQUAL(shape_we_got, shape_we_want);
    }

    {
        Count const nr_worker_threads{6};
        Shape const array_shape{5};
        Shape const shape_we_want{1};

        Shape const shape_we_got{lue::default_partition_shape(array_shape, nr_worker_threads)};

        BOOST_CHECK_EQUAL(shape_we_got, shape_we_want);
    }

    {
        Count const nr_worker_threads{5};
        Shape const array_shape{499};
        Shape const shape_we_want{99};

        Shape const shape_we_got{lue::default_partition_shape(array_shape, nr_worker_threads)};

        BOOST_CHECK_EQUAL(shape_we_got, shape_we_want);
    }

    {
        Count const nr_worker_threads{5};
        Shape const array_shape{500};
        Shape const shape_we_want{100};

        Shape const shape_we_got{lue::default_partition_shape(array_shape, nr_worker_threads)};

        BOOST_CHECK_EQUAL(shape_we_got, shape_we_want);
    }

    {
        Count const nr_worker_threads{5};
        Shape const array_shape{501};
        Shape const shape_we_want{100};

        Shape const shape_we_got{lue::default_partition_shape(array_shape, nr_worker_threads)};

        BOOST_CHECK_EQUAL(shape_we_got, shape_we_want);
    }

    {
        Count const nr_worker_threads{5};
        Shape const array_shape{504};
        Shape const shape_we_want{100};

        Shape const shape_we_got{lue::default_partition_shape(array_shape, nr_worker_threads)};

        BOOST_CHECK_EQUAL(shape_we_got, shape_we_want);
    }

    {
        Count const nr_worker_threads{5};
        Shape const array_shape{505};
        Shape const shape_we_want{101};

        Shape const shape_we_got{lue::default_partition_shape(array_shape, nr_worker_threads)};

        BOOST_CHECK_EQUAL(shape_we_got, shape_we_want);
    }

    // ----- Normal cases -----
    {
        Count const nr_worker_threads{5};
        Shape const array_shape{nr_worker_threads * default_partition_size};
        Shape const shape_we_want{default_partition_size};

        Shape const shape_we_got{lue::default_partition_shape(array_shape, nr_worker_threads)};

        BOOST_CHECK_EQUAL(shape_we_got, shape_we_want);
    }

    {
        Count const nr_worker_threads{5};
        Shape const array_shape{(nr_worker_threads * default_partition_size) - 1};
        Shape const shape_we_want{default_partition_size - 1};

        Shape const shape_we_got{lue::default_partition_shape(array_shape, nr_worker_threads)};

        BOOST_CHECK_EQUAL(shape_we_got, shape_we_want);
    }

    {
        Count const nr_worker_threads{5};
        Shape const array_shape{(nr_worker_threads * default_partition_size) + 1};
        Shape const shape_we_want{default_partition_size};

        Shape const shape_we_got{lue::default_partition_shape(array_shape, nr_worker_threads)};

        BOOST_CHECK_EQUAL(shape_we_got, shape_we_want);
    }
}


BOOST_AUTO_TEST_CASE(default_partition_shape_2d)
{
    lue::Count const default_partition_size{2000 * 2000};
    lue::Rank const rank{2};
    using Shape = lue::Shape<lue::Index, rank>;
    using Count = lue::Count;

    // ----- Empty and small stuff -----
    {
        Count const nr_worker_threads{1};
        Shape const array_shape{0, 0};
        Shape const shape_we_want{0, 0};

        Shape const shape_we_got{lue::default_partition_shape(array_shape, nr_worker_threads)};

        BOOST_CHECK_EQUAL(shape_we_got, shape_we_want);
    }

    {
        Count const nr_worker_threads{1};
        Shape const array_shape{1, 1};
        Shape const shape_we_want{1, 1};

        Shape const shape_we_got{lue::default_partition_shape(array_shape, nr_worker_threads)};

        BOOST_CHECK_EQUAL(shape_we_got, shape_we_want);
    }

    {
        Count const nr_worker_threads{2};
        Shape const array_shape{1, 1};
        Shape const shape_we_want{1, 1};

        Shape const shape_we_got{lue::default_partition_shape(array_shape, nr_worker_threads)};

        BOOST_CHECK_EQUAL(shape_we_got, shape_we_want);
    }

    {
        Count const nr_worker_threads{6};
        Shape const array_shape{3, 3};
        Shape const shape_we_want{1, 1};

        Shape const shape_we_got{lue::default_partition_shape(array_shape, nr_worker_threads)};

        BOOST_CHECK_EQUAL(shape_we_got, shape_we_want);
    }

    // ----- Normal cases -----
    {
        Count const nr_worker_threads{5};
        Shape const array_shape{
            nr_worker_threads * default_partition_size, nr_worker_threads * default_partition_size};
        Shape const shape_we_want{
            static_cast<Count>(std::sqrt(default_partition_size)),
            static_cast<Count>(std::sqrt(default_partition_size))};

        Shape const shape_we_got{lue::default_partition_shape(array_shape, nr_worker_threads)};

        BOOST_CHECK_EQUAL(shape_we_got, shape_we_want);
    }

    {
        Count const nr_worker_threads{1};
        Shape const array_shape{2000, 1000};
        Shape const shape_we_want{1000, 1000};
        Shape const shape_we_got{lue::default_partition_shape(array_shape, nr_worker_threads)};

        BOOST_CHECK_EQUAL(shape_we_got, shape_we_want);
    }
}


BOOST_AUTO_TEST_CASE(is_subset_of)
{
    lue::Rank const rank{2};
    using Shape = lue::Shape<lue::Index, rank>;

    BOOST_CHECK(lue::detail::is_subset_of(Shape{10000, 10000}, Shape{10000, 10000}));
    BOOST_CHECK(lue::detail::is_subset_of(Shape{500, 10000}, Shape{10000, 10000}));
    BOOST_CHECK(lue::detail::is_subset_of(Shape{10000, 500}, Shape{10000, 10000}));
    BOOST_CHECK(lue::detail::is_subset_of(Shape{500, 500}, Shape{10000, 10000}));

    BOOST_CHECK(!lue::detail::is_subset_of(Shape{10500, 10500}, Shape{10000, 10000}));
    BOOST_CHECK(!lue::detail::is_subset_of(Shape{500, 10500}, Shape{10000, 10000}));
    BOOST_CHECK(!lue::detail::is_subset_of(Shape{10500, 500}, Shape{10000, 10000}));

    BOOST_CHECK(!lue::detail::is_subset_of(Shape{1414, 1414}, Shape{2000, 1000}));
}
