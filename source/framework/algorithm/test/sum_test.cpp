#define BOOST_TEST_MODULE lue framework algorithm sum
#include "lue/framework/algorithm/create_partitioned_array.hpp"
#include "lue/framework/algorithm/value_policies/sum.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"


namespace detail {

    template<typename Element, typename ResultElement, std::size_t rank>
    void test_array()
    {
        using Array = lue::PartitionedArray<Element, rank>;

        auto const array_shape{lue::Test<Array>::shape()};
        auto const partition_shape{lue::Test<Array>::partition_shape()};

        Element const fill_value{5};

        Array array{lue::create_partitioned_array(array_shape, partition_shape, fill_value)};

        auto sum = lue::value_policies::sum<ResultElement>(array);

        using TypeWeGot = decltype(sum);
        using TypeWeWant = hpx::future<ResultElement>;
        static_assert(std::is_same_v<TypeWeGot, TypeWeWant>);

        BOOST_CHECK_EQUAL(sum.get(), static_cast<Element>(lue::nr_elements(array_shape) * fill_value));
    }


    template<typename Element, typename ResultElement>
    void test_array_1d()
    {
        test_array<Element, ResultElement, 1>();
    }


    template<typename Element, typename ResultElement>
    void test_array_2d()
    {
        test_array<Element, ResultElement, 2>();
    }

}  // namespace detail


#define TEST_CASE(rank, Element, ResultElement)                                                              \
                                                                                                             \
    BOOST_AUTO_TEST_CASE(array_##rank##d_##Element##_##ResultElement)                                        \
    {                                                                                                        \
        detail::test_array_##rank##d<Element, ResultElement>();                                              \
    }

// TEST_CASE(1, int32_t, int32_t)
TEST_CASE(2, int32_t, int32_t)
// TEST_CASE(1, int64_t, int64_t)
// TEST_CASE(2, int64_t, int64_t)
// TEST_CASE(1, int32_t, int64_t)
// TEST_CASE(2, int32_t, int64_t)

#undef TEST_CASE
