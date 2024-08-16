#define BOOST_TEST_MODULE lue framework algorithm all
#include "lue/framework/algorithm/create_partitioned_array.hpp"
#include "lue/framework/algorithm/default_policies/all.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"


namespace detail {

    template<typename Element, std::size_t rank>
    void test_array()
    {
        using namespace lue::default_policies;

        using Array = lue::PartitionedArray<Element, rank>;

        auto const array_shape{lue::Test<Array>::shape()};
        auto const partition_shape{lue::Test<Array>::partition_shape()};

        Element const fill_value1{1};  // true
        Element const fill_value2{0};  // false

        Array array1{lue::create_partitioned_array(array_shape, partition_shape, fill_value1)};
        Array array2{lue::create_partitioned_array(array_shape, partition_shape, fill_value2)};

        BOOST_CHECK_EQUAL(all(array1).future().get(), Element{1});  // true
        BOOST_CHECK_EQUAL(all(array2).future().get(), Element{0});  // false
    }

}  // namespace detail


#define TEST_CASE(rank, Element)                                                                             \
                                                                                                             \
    BOOST_AUTO_TEST_CASE(array_##rank##d_##Element)                                                          \
    {                                                                                                        \
        detail::test_array<Element, rank>();                                                                 \
    }

TEST_CASE(1, bool)
TEST_CASE(1, int32_t)
TEST_CASE(2, bool)
TEST_CASE(2, int32_t)

#undef TEST_CASE
