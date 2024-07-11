#define BOOST_TEST_MODULE lue framework api cxx local uniform
#include "lue/framework/api/cxx/array.hpp"
#include "lue/framework/api/cxx/local/uniform.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"


BOOST_AUTO_TEST_CASE(uniform_element_element)
{
    using Element = std::int32_t;
    using Array = lue::PartitionedArray<Element, 2>;
    using Shape = lue::ShapeT<Array>;

    Shape const array_shape{1, 1};
    Shape const partition_shape{1, 1};

    lue::api::Field const min_value = Element{};
    lue::api::Field const max_value = Element{};

    lue::api::Field result = lue::api::uniform(array_shape, partition_shape, min_value, max_value);

    BOOST_CHECK(std::holds_alternative<Array>(result));
}


BOOST_AUTO_TEST_CASE(uniform_without_partition_shape)
{
    using Element = std::int32_t;
    using Array = lue::PartitionedArray<Element, 2>;
    using Shape = lue::ShapeT<Array>;

    Shape const array_shape{1, 1};

    lue::api::Field const min_value = Element{};
    lue::api::Field const max_value = Element{};

    lue::api::Field result = lue::api::uniform(array_shape, min_value, max_value);

    BOOST_CHECK(std::holds_alternative<Array>(result));
}
