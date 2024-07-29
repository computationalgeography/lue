#define BOOST_TEST_MODULE lue framework algorithm verify_compatible
#include "lue/framework/algorithm/detail/verify_compatible.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"


BOOST_AUTO_TEST_CASE(compatible)
{
    using Element = std::int32_t;
    constexpr lue::Rank rank = 2;
    using Shape = lue::Shape<lue::Count, rank>;

    Shape const array_shape{{60, 40}};
    Shape const partition_shape{{10, 10}};

    auto array1 = lue::create_partitioned_array<Element>(array_shape, partition_shape, 5);
    auto array2 = lue::create_partitioned_array<Element>(array_shape, partition_shape, 6);
    auto array3 = lue::create_partitioned_array<Element>(array_shape, partition_shape, 7);

    BOOST_CHECK_NO_THROW(lue::detail::verify_compatible(array1));
    BOOST_CHECK_NO_THROW(lue::detail::verify_compatible(array1, array2));
    BOOST_CHECK_NO_THROW(lue::detail::verify_compatible(array1, array2, array3));

    BOOST_CHECK_NO_THROW(lue::detail::verify_compatible(array1, array1));
    BOOST_CHECK_NO_THROW(lue::detail::verify_compatible(array1, array1, array1));
}


BOOST_AUTO_TEST_CASE(not_compatible_shape_in_elements)
{
    using Element = std::int32_t;
    constexpr lue::Rank rank = 2;
    using Shape = lue::Shape<lue::Count, rank>;

    Shape const partition_shape{{10, 10}};

    auto array1 = lue::create_partitioned_array<Element>({{60, 40}}, partition_shape, 5);
    auto array2 = lue::create_partitioned_array<Element>({{40, 60}}, partition_shape, 6);
    auto array3 = lue::create_partitioned_array<Element>({{30, 30}}, partition_shape, 7);

    BOOST_CHECK_THROW(lue::detail::verify_compatible(array1, array2), std::runtime_error);
    BOOST_CHECK_THROW(lue::detail::verify_compatible(array1, array2, array3), std::runtime_error);

    BOOST_CHECK_THROW(lue::detail::verify_compatible(array1, array1, array2), std::runtime_error);
    BOOST_CHECK_THROW(lue::detail::verify_compatible(array2, array1, array1), std::runtime_error);
}


BOOST_AUTO_TEST_CASE(not_compatible_shape_in_partitions)
{
    using Element = std::int32_t;
    constexpr lue::Rank rank = 2;
    using Shape = lue::Shape<lue::Count, rank>;

    Shape const array_shape{{60, 40}};

    auto array1 = lue::create_partitioned_array<Element>(array_shape, {{10, 10}}, 5);
    auto array2 = lue::create_partitioned_array<Element>(array_shape, {{5, 10}}, 6);
    auto array3 = lue::create_partitioned_array<Element>(array_shape, {{10, 5}}, 7);

    BOOST_CHECK_THROW(lue::detail::verify_compatible(array1, array2), std::runtime_error);
    BOOST_CHECK_THROW(lue::detail::verify_compatible(array1, array2, array3), std::runtime_error);

    BOOST_CHECK_THROW(lue::detail::verify_compatible(array1, array1, array2), std::runtime_error);
    BOOST_CHECK_THROW(lue::detail::verify_compatible(array2, array1, array1), std::runtime_error);
}
