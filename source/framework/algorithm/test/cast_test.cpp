#define BOOST_TEST_MODULE lue framework algorithm cast
#include "lue/framework/algorithm/all.hpp"
#include "lue/framework/algorithm/array_like.hpp"
#include "lue/framework/algorithm/cast.hpp"
#include "lue/framework/algorithm/definition/equal_to.hpp"
#include "lue/framework/algorithm/create_partitioned_array.hpp"
#include "lue/framework/test/array.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"


namespace detail {

    template<
        typename InputElement,
        typename OutputElement,
        std::size_t rank>
    void test_array()
    {
        using InputArray = lue::PartitionedArray<InputElement, rank>;

        auto const array_shape{lue::Test<InputArray>::shape()};
        auto const partition_shape{lue::Test<InputArray>::partition_shape()};

        InputElement const fill_value(5);

        InputArray array{lue::create_partitioned_array(array_shape, partition_shape, fill_value)};

        // Request the cast of the array
        auto cast = lue::cast<OutputElement>(array);

        BOOST_CHECK(
            lue::all(cast == lue::array_like(array, static_cast<OutputElement>(fill_value))).get());
    }

}  // namespace detail


#define TEST_CASE(                                                      \
    rank,                                                               \
    InputElement,                                                       \
    OutputElement)                                                      \
                                                                        \
BOOST_AUTO_TEST_CASE(array_##rank##d_##InputElement##_##OutputElement)  \
{                                                                       \
    detail::test_array<InputElement, OutputElement, rank>();            \
}

// TEST_CASE(1, bool, bool)
// TEST_CASE(2, bool, bool)
TEST_CASE(1, bool, int32_t)
TEST_CASE(2, bool, int32_t)
TEST_CASE(1, bool, double)
TEST_CASE(2, bool, double)
// TEST_CASE(1, int32_t, bool)
// TEST_CASE(2, int32_t, bool)
TEST_CASE(1, int32_t, int32_t)
TEST_CASE(2, int32_t, int32_t)
TEST_CASE(1, int32_t, double)
TEST_CASE(2, int32_t, double)
// TEST_CASE(1, int64_t)
// TEST_CASE(2, int64_t)
// TEST_CASE(1, float)
// TEST_CASE(2, float)
// TEST_CASE(1, double, bool)
// TEST_CASE(2, double, bool)
TEST_CASE(1, double, int32_t)
TEST_CASE(2, double, int32_t)
TEST_CASE(1, double, double)
TEST_CASE(2, double, double)

#undef TEST_CASE
