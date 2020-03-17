#define BOOST_TEST_MODULE lue framework algorithm where
#include "lue/framework/algorithm/where.hpp"
#include "lue/framework/test/compare.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"


template<
    typename Element,
    lue::Rank rank>
using Array = lue::PartitionedArray<Element, rank>;

template<
    typename Element,
    lue::Rank rank>
using Partition = lue::PartitionT<Array<Element, rank>>;

template<
    typename Element,
    lue::Rank rank>
using PartitionData = lue::DataT<Partition<Element, rank>>;


BOOST_AUTO_TEST_CASE(use_case_1)
{
    // Array<Element> = where(Array<bool>, Array<Element>, Element)

    // Condition:     True array:     False value:
    // +-----+-----+  +-----+-----+
    // |  T  |  F  |  |  1  |  2  |   9
    // +-----+-----+  +-----+-----+
    // |  F  |  T  |  |  3  |  4  |
    // +-----+-----+  +-----+-----+
    // |  T  |  F  |  |  5  |  6  |
    // +-----+-----+  +-----+-----+
    lue::Count const nr_rows{3};
    lue::Count const nr_cols{2};

    using Element = std::int32_t;
    lue::Rank const rank{2};

    using Shape = lue::ShapeT<Array<Element, rank>>;

    Shape shape{nr_rows, nr_cols};

    PartitionData<bool, rank> condition_data{
            shape,
            std::initializer_list<bool>{
                true, false,
                false, true,
                true, false,
            }
        };
    Array<bool, rank> condition{shape};
    condition.partitions()(0, 0).set_data(std::move(condition_data));

    PartitionData<Element, rank> true_data{
            shape,
            std::initializer_list<Element>{
                1, 2,
                3, 4,
                5, 6,
            }
        };
    Array<Element, rank> true_array{shape};
    true_array.partitions()(0, 0).set_data(std::move(true_data));

    Element const false_value{9};

    auto result_we_got = lue::where(condition, true_array, false_value);

    PartitionData<Element, rank> result_we_want_data{
            shape,
            std::initializer_list<Element>{
                1, 9,
                9, 4,
                5, 9,
            }
        };
    Array<Element, rank> result_we_want{shape};
    result_we_want.partitions()(0, 0).set_data(std::move(result_we_want_data));

    lue::test::check_arrays_are_equal(result_we_got, result_we_want);
}
