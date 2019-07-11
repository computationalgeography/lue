#define BOOST_TEST_MODULE lue framework algorithm cast
#include "lue/framework/algorithm/cast.hpp"
#include "lue/framework/algorithm/fill.hpp"
#include "lue/framework/algorithm/sum.hpp"
#include "lue/framework/core/component/partitioned_array.hpp"
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

    auto const shape{lue::Test<InputArray>::shape()};

    InputArray array{shape};
    hpx::shared_future<InputElement> fill_value =
        hpx::make_ready_future<InputElement>(5);

    // Request the filling of the array and wait for it to finish
    lue::fill(array, fill_value).wait();

    // Request the cast of the array
    auto cast = lue::cast<InputArray, OutputElement>(array);

    // Request the sumation of the array
    auto sum = lue::sum(cast);

    BOOST_CHECK_EQUAL(
        sum.get(),
        static_cast<OutputElement>(
            lue::nr_elements(shape) *
            static_cast<OutputElement>(fill_value.get())));
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
