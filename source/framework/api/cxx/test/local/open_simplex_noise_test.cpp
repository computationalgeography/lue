#define BOOST_TEST_MODULE lue framework api cxx local open_simplex_noise
#include "lue/framework/api/cxx/local/open_simplex_noise.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"
#include "lue/framework.hpp"


BOOST_AUTO_TEST_CASE(open_simplex_noise_array_array)
{
    using Element = lue::SmallestFloatingPointElement;
    using Array = lue::PartitionedArray<Element, 2>;

    lue::api::Field const array = Array{};
    int const seed{};

    lue::api::Field result = lue::api::open_simplex_noise(array, array, seed);

    BOOST_CHECK(std::holds_alternative<Array>(result.variant()));
}
