#define BOOST_TEST_MODULE lue framework algorithm add
#include "lue/framework/algorithm/create_partitioned_array.hpp"
#include "lue/framework/algorithm/default_policies/add.hpp"
#include "lue/framework/algorithm/default_policies/all.hpp"
#include "lue/framework/algorithm/default_policies/equal_to.hpp"
#include "lue/framework/algorithm/value_policies/add.hpp"
#include "lue/framework/algorithm/value_policies/none.hpp"
#include "lue/framework/algorithm/value_policies/valid.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"
#include "lue/framework.hpp"


namespace {

    template<typename Element, std::size_t rank>
    void test_array()
    {
        using namespace lue::default_policies;

        using Array = lue::PartitionedArray<Element, rank>;

        auto const array_shape{lue::Test<Array>::shape()};
        auto const partition_shape{lue::Test<Array>::partition_shape()};

        Element const fill_value1{5};
        Element const fill_value2{6};

        Array array1{lue::create_partitioned_array(array_shape, partition_shape, fill_value1)};
        Array array2{lue::create_partitioned_array(array_shape, partition_shape, fill_value2)};

        Array add = array1 + array2;

        BOOST_CHECK(all(add == Element{fill_value1 + fill_value2}).future().get());
    }

}  // Anonymous namespace


BOOST_AUTO_TEST_CASE(use_case_01)
{
    lue::Rank const rank{2};

    test_array<lue::SignedIntegralElement<0>, rank>();
    test_array<lue::FloatingPointElement<0>, rank>();
}


BOOST_AUTO_TEST_CASE(out_of_range)
{
    using namespace lue::value_policies;

    using Element = lue::SignedIntegralElement<0>;
    using BooleanElement = lue::BooleanElement;
    lue::Rank const rank{2};
    using Scalar = lue::Scalar<Element>;
    using Array = lue::PartitionedArray<Element, rank>;

    auto const array_shape{lue::Test<Array>::shape()};
    auto const partition_shape{lue::Test<Array>::partition_shape()};

    Element const max{std::numeric_limits<Element>::max()};
    Element const fill_value{max};

    Array array{lue::create_partitioned_array(array_shape, partition_shape, fill_value)};

    // Careful. The default no-data value for int32_t is the lowest value. Although max +
    // 1 is undefined for signed integers, this may wrap around to this minimum value. Therefore,
    // we add 2 here, instead of 1. Adding 1 confuses things.
    BOOST_CHECK(none(valid<BooleanElement>(Element{2} + array)).future().get());
    BOOST_CHECK(none(valid<BooleanElement>(array + Element{2})).future().get());
    BOOST_CHECK(none(valid<BooleanElement>(array + array)).future().get());
    BOOST_CHECK_EQUAL((valid<BooleanElement>(Scalar{max} + Element{2})).future().get(), 0);
}


BOOST_AUTO_TEST_CASE(value_icw_scalar)
{
    using namespace lue::value_policies;

    using Value = lue::SignedIntegralElement<0>;
    using BooleanElement = lue::BooleanElement;
    using Scalar = lue::Scalar<Value>;

    Value const max{std::numeric_limits<Value>::max()};

    Value const value{5};
    Scalar const scalar{6};

    BOOST_CHECK_EQUAL((value + scalar).future().get(), 11);
    BOOST_CHECK_EQUAL((scalar + value).future().get(), 11);
    BOOST_CHECK(!valid<BooleanElement>(scalar + max).future().get());
}
