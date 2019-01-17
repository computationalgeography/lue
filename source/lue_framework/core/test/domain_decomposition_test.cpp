#define BOOST_TEST_MODULE lue framework core domain_decomposition
#include <boost/test/unit_test.hpp>
#include "lue/framework/core/domain_decomposition.hpp"


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


BOOST_AUTO_TEST_CASE(partitions)
{
    using Index = std::uint64_t;
    std::size_t const rank = 2;
    using Partition = lue::ArrayPartitionDefinition<Index, rank>;
    using Start = typename Partition::Start;
    using Shape = typename Partition::Shape;

    {
        Shape area_shape{6, 4};
        Shape partition_shape{6, 4};

        std::size_t nr_localities = 2;
        std::uint32_t locality_id = 0;

        auto partitions = lue::partitions(
            area_shape, partition_shape, nr_localities, locality_id);

        BOOST_REQUIRE_EQUAL(partitions.size(), 1);
        BOOST_CHECK(partitions[0] == Partition{partition_shape});

        locality_id = 1;
        partitions = lue::partitions(
            area_shape, partition_shape, nr_localities, locality_id);

        BOOST_REQUIRE_EQUAL(partitions.size(), 0);
    }

    {
        // Grains fit perfectly in area
        Shape area_shape{60, 40};
        Shape partition_shape{10, 10};

        std::size_t nr_localities = 6;

        std::uint32_t locality_id = 0;
        auto partitions = lue::partitions(
            area_shape, partition_shape, nr_localities, locality_id);

        BOOST_REQUIRE_EQUAL(partitions.size(), 4);
        BOOST_CHECK(partitions[0] == (Partition{Start{0,  0}, Shape{10, 10}}));
        BOOST_CHECK(partitions[1] == (Partition{Start{0, 10}, Shape{10, 10}}));
        BOOST_CHECK(partitions[2] == (Partition{Start{0, 20}, Shape{10, 10}}));
        BOOST_CHECK(partitions[3] == (Partition{Start{0, 30}, Shape{10, 10}}));

        locality_id = 5;
        partitions = lue::partitions(
            area_shape, partition_shape, nr_localities, locality_id);

        BOOST_REQUIRE_EQUAL(partitions.size(), 4);
        BOOST_CHECK(partitions[0] == (Partition{Start{50,  0}, Shape{10, 10}}));
        BOOST_CHECK(partitions[1] == (Partition{Start{50, 10}, Shape{10, 10}}));
        BOOST_CHECK(partitions[2] == (Partition{Start{50, 20}, Shape{10, 10}}));
        BOOST_CHECK(partitions[3] == (Partition{Start{50, 30}, Shape{10, 10}}));
    }

    {
        // Grains don't fit perfectly in area
        Shape area_shape{60, 40};
        Shape partition_shape{9, 9};

        std::size_t nr_localities = 6;

        std::uint32_t locality_id = 0;
        auto partitions = lue::partitions(
            area_shape, partition_shape, nr_localities, locality_id);

        BOOST_REQUIRE_EQUAL(partitions.size(), 6);
        BOOST_CHECK(partitions[0] == (Partition{Start{0,  0}, Shape{9, 9}}));
        BOOST_CHECK(partitions[1] == (Partition{Start{0,  9}, Shape{9, 9}}));
        BOOST_CHECK(partitions[2] == (Partition{Start{0, 18}, Shape{9, 9}}));
        BOOST_CHECK(partitions[3] == (Partition{Start{0, 27}, Shape{9, 9}}));
        BOOST_CHECK(partitions[4] == (Partition{Start{0, 36}, Shape{9, 4}}));
        BOOST_CHECK(partitions[5] == (Partition{Start{9,  0}, Shape{9, 9}}));

        locality_id = 4;
        partitions = lue::partitions(
            area_shape, partition_shape, nr_localities, locality_id);

        BOOST_REQUIRE_EQUAL(partitions.size(), 6);
        BOOST_CHECK(partitions[0] == (Partition{Start{36, 36}, Shape{9, 4}}));
        BOOST_CHECK(partitions[1] == (Partition{Start{45,  0}, Shape{9, 9}}));
        BOOST_CHECK(partitions[2] == (Partition{Start{45,  9}, Shape{9, 9}}));
        BOOST_CHECK(partitions[3] == (Partition{Start{45, 18}, Shape{9, 9}}));
        BOOST_CHECK(partitions[4] == (Partition{Start{45, 27}, Shape{9, 9}}));
        BOOST_CHECK(partitions[5] == (Partition{Start{45, 36}, Shape{9, 4}}));

        locality_id = 5;
        partitions = lue::partitions(
            area_shape, partition_shape, nr_localities, locality_id);

        BOOST_REQUIRE_EQUAL(partitions.size(), 5);
        BOOST_CHECK(partitions[0] == (Partition{Start{54,  0}, Shape{6, 9}}));
        BOOST_CHECK(partitions[1] == (Partition{Start{54,  9}, Shape{6, 9}}));
        BOOST_CHECK(partitions[2] == (Partition{Start{54, 18}, Shape{6, 9}}));
        BOOST_CHECK(partitions[3] == (Partition{Start{54, 27}, Shape{6, 9}}));
        BOOST_CHECK(partitions[4] == (Partition{Start{54, 36}, Shape{6, 4}}));
    }

    {
        // Grains don't fit perfectly in area
        Shape area_shape{60, 40};
        Shape partition_shape{11, 11};

        std::size_t nr_localities = 6;

        std::uint32_t locality_id = 0;
        auto partitions = lue::partitions(
            area_shape, partition_shape, nr_localities, locality_id);

        BOOST_REQUIRE_EQUAL(partitions.size(), 4);
        BOOST_CHECK(partitions[0] == (Partition{Start{0,  0}, Shape{11, 11}}));
        BOOST_CHECK(partitions[1] == (Partition{Start{0, 11}, Shape{11, 11}}));
        BOOST_CHECK(partitions[2] == (Partition{Start{0, 22}, Shape{11, 11}}));
        BOOST_CHECK(partitions[3] == (Partition{Start{0, 33}, Shape{11,  7}}));

        locality_id = 5;
        partitions = lue::partitions(
            area_shape, partition_shape, nr_localities, locality_id);

        BOOST_REQUIRE_EQUAL(partitions.size(), 4);
        BOOST_CHECK(partitions[0] == (Partition{Start{55,  0}, Shape{5, 11}}));
        BOOST_CHECK(partitions[1] == (Partition{Start{55, 11}, Shape{5, 11}}));
        BOOST_CHECK(partitions[2] == (Partition{Start{55, 22}, Shape{5, 11}}));
        BOOST_CHECK(partitions[3] == (Partition{Start{55, 33}, Shape{5,  7}}));
    }
}
