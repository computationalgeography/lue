#define BOOST_TEST_MODULE lue framework algorithm cast
#include "lue/framework/algorithm/array_like.hpp"
#include "lue/framework/algorithm/create_partitioned_array.hpp"
#include "lue/framework/algorithm/default_policies/all.hpp"
#include "lue/framework/algorithm/default_policies/cast.hpp"
#include "lue/framework/algorithm/default_policies/equal_to.hpp"
#include "lue/framework/test/array.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"


namespace detail {

    template<typename InputElement, typename OutputElement, std::size_t rank>
    void test_array()
    {
        using namespace lue::default_policies;

        using InputArray = lue::PartitionedArray<InputElement, rank>;

        auto const array_shape{lue::Test<InputArray>::shape()};
        auto const partition_shape{lue::Test<InputArray>::partition_shape()};

        InputElement const fill_value(5);

        InputArray array{lue::create_partitioned_array(array_shape, partition_shape, fill_value)};

        // Request the cast of the array
        auto array_we_got = cast<OutputElement>(array);
        auto array_we_want = lue::array_like(array, static_cast<OutputElement>(fill_value));

        BOOST_CHECK(all(array_we_got == array_we_want).get());
    }

}  // namespace detail


#define TEST_CASE(rank, InputElement, OutputElement)                                                         \
                                                                                                             \
    BOOST_AUTO_TEST_CASE(array_##rank##d_##InputElement##_##OutputElement)                                   \
    {                                                                                                        \
        detail::test_array<InputElement, OutputElement, rank>();                                             \
    }

// TEST_CASE(2, bool, int32_t)
// TEST_CASE(2, bool, uint32_t)
// TEST_CASE(2, bool, int64_t)
// TEST_CASE(2, bool, uint64_t)
// TEST_CASE(2, bool, float)
// TEST_CASE(2, bool, double)

// TEST_CASE(2, int32_t, int64_t)
// TEST_CASE(2, int32_t, float)
// TEST_CASE(2, int32_t, double)

// TEST_CASE(2, uint32_t, uint64_t)
// TEST_CASE(2, uint32_t, float)
// TEST_CASE(2, uint32_t, double)

TEST_CASE(2, int64_t, float)
TEST_CASE(2, int64_t, double)

TEST_CASE(2, uint64_t, float)
TEST_CASE(2, uint64_t, double)

#undef TEST_CASE
