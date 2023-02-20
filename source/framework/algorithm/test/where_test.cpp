#define BOOST_TEST_MODULE lue framework algorithm where
#include "lue/framework/algorithm/create_partitioned_array.hpp"
#include "lue/framework/algorithm/default_policies/where.hpp"
#include "lue/framework/algorithm/value_policies/where.hpp"
#include "lue/framework/test/compare.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"


namespace {

    template<typename Element, lue::Rank rank>
    using Array = lue::PartitionedArray<Element, rank>;

    template<typename Element, lue::Rank rank>
    using Partition = lue::PartitionT<Array<Element, rank>>;

    template<typename Element, lue::Rank rank>
    using PartitionData = lue::DataT<Partition<Element, rank>>;


    template<typename ConditionElement, typename Element, lue::Rank rank>
    void test_where(
        lue::ShapeT<Array<Element, rank>> const& shape,
        std::initializer_list<ConditionElement>&& condition_values,
        std::initializer_list<Element>&& true_values,
        std::initializer_list<Element>&& result_values_we_want)
    {
        PartitionData<ConditionElement, rank> condition_data{shape, condition_values};
        Array<ConditionElement, rank> condition{
            lue::create_partitioned_array<ConditionElement>(shape, shape)};
        condition.partitions()(0, 0).wait();
        condition.partitions()(0, 0).set_data(std::move(condition_data)).wait();

        PartitionData<Element, rank> true_data{shape, true_values};
        Array<Element, rank> true_array{lue::create_partitioned_array<Element>(shape, shape)};
        true_array.partitions()(0, 0).wait();
        true_array.partitions()(0, 0).set_data(std::move(true_data)).wait();

        auto result_we_got = lue::value_policies::where(condition, true_array);

        PartitionData<Element, rank> result_we_want_data{shape, result_values_we_want};
        Array<Element, rank> result_we_want{lue::create_partitioned_array<Element>(shape, shape)};
        result_we_want.partitions()(0, 0).wait();
        result_we_want.partitions()(0, 0).set_data(std::move(result_we_want_data)).wait();

        lue::test::check_arrays_are_equal(result_we_got, result_we_want);
    }


    template<typename ConditionElement, typename Element, lue::Rank rank>
    void test_where(
        lue::ShapeT<Array<Element, rank>> const& shape,
        std::initializer_list<ConditionElement>&& condition_values,
        Element true_value,
        std::initializer_list<Element>&& result_values_we_want)
    {
        PartitionData<ConditionElement, rank> condition_data{shape, condition_values};
        Array<ConditionElement, rank> condition{
            lue::create_partitioned_array<ConditionElement>(shape, shape)};
        condition.partitions()(0, 0).wait();
        condition.partitions()(0, 0).set_data(std::move(condition_data)).wait();

        auto result_we_got = lue::value_policies::where(condition, true_value);

        PartitionData<Element, rank> result_we_want_data{shape, result_values_we_want};
        Array<Element, rank> result_we_want{lue::create_partitioned_array<Element>(shape, shape)};
        result_we_want.partitions()(0, 0).wait();
        result_we_want.partitions()(0, 0).set_data(std::move(result_we_want_data)).wait();

        lue::test::check_arrays_are_equal(result_we_got, result_we_want);
    }


    template<typename ConditionElement, typename Element, lue::Rank rank>
    void test_where(
        lue::ShapeT<Array<Element, rank>> const& shape,
        std::initializer_list<ConditionElement>&& condition_values,
        std::initializer_list<Element>&& true_values,
        std::initializer_list<Element>&& false_values,
        std::initializer_list<Element>&& result_values_we_want)
    {
        PartitionData<ConditionElement, rank> condition_data{shape, condition_values};
        Array<ConditionElement, rank> condition{
            lue::create_partitioned_array<ConditionElement>(shape, shape)};
        condition.partitions()(0, 0).wait();
        condition.partitions()(0, 0).set_data(std::move(condition_data)).wait();

        PartitionData<Element, rank> true_data{shape, true_values};
        Array<Element, rank> true_array{lue::create_partitioned_array<Element>(shape, shape)};
        true_array.partitions()(0, 0).wait();
        true_array.partitions()(0, 0).set_data(std::move(true_data)).wait();

        PartitionData<Element, rank> false_data{shape, false_values};
        Array<Element, rank> false_array{lue::create_partitioned_array<Element>(shape, shape)};
        false_array.partitions()(0, 0).wait();
        false_array.partitions()(0, 0).set_data(std::move(false_data)).wait();

        auto result_we_got = lue::value_policies::where(condition, true_array, false_array);

        PartitionData<Element, rank> result_we_want_data{shape, result_values_we_want};
        Array<Element, rank> result_we_want{lue::create_partitioned_array<Element>(shape, shape)};
        result_we_want.partitions()(0, 0).wait();
        result_we_want.partitions()(0, 0).set_data(std::move(result_we_want_data)).wait();

        lue::test::check_arrays_are_equal(result_we_got, result_we_want);
    }


    template<typename ConditionElement, typename Element, lue::Rank rank>
    void test_where(
        lue::ShapeT<Array<Element, rank>> const& shape,
        std::initializer_list<ConditionElement>&& condition_values,
        std::initializer_list<Element>&& true_values,
        Element const false_value,
        std::initializer_list<Element>&& result_values_we_want)
    {
        PartitionData<ConditionElement, rank> condition_data{shape, condition_values};
        Array<ConditionElement, rank> condition{
            lue::create_partitioned_array<ConditionElement>(shape, shape)};
        condition.partitions()(0, 0).wait();
        condition.partitions()(0, 0).set_data(std::move(condition_data)).wait();

        PartitionData<Element, rank> true_data{shape, true_values};
        Array<Element, rank> true_array{lue::create_partitioned_array<Element>(shape, shape)};
        true_array.partitions()(0, 0).wait();
        true_array.partitions()(0, 0).set_data(std::move(true_data)).wait();

        auto result_we_got = lue::value_policies::where(condition, true_array, false_value);

        PartitionData<Element, rank> result_we_want_data{shape, result_values_we_want};
        Array<Element, rank> result_we_want{lue::create_partitioned_array<Element>(shape, shape)};
        result_we_want.partitions()(0, 0).wait();
        result_we_want.partitions()(0, 0).set_data(std::move(result_we_want_data)).wait();

        lue::test::check_arrays_are_equal(result_we_got, result_we_want);
    }


    template<typename ConditionElement, typename Element, lue::Rank rank>
    void test_where(
        lue::ShapeT<Array<Element, rank>> const& shape,
        std::initializer_list<ConditionElement>&& condition_values,
        Element const true_value,
        std::initializer_list<Element>&& false_values,
        std::initializer_list<Element>&& result_values_we_want)
    {
        PartitionData<ConditionElement, rank> condition_data{shape, condition_values};
        Array<ConditionElement, rank> condition{
            lue::create_partitioned_array<ConditionElement>(shape, shape)};
        condition.partitions()(0, 0).wait();
        condition.partitions()(0, 0).set_data(std::move(condition_data)).wait();

        PartitionData<Element, rank> false_data{shape, false_values};
        Array<Element, rank> false_array{lue::create_partitioned_array<Element>(shape, shape)};
        false_array.partitions()(0, 0).wait();
        false_array.partitions()(0, 0).set_data(std::move(false_data)).wait();

        auto result_we_got = lue::value_policies::where(condition, true_value, false_array);

        PartitionData<Element, rank> result_we_want_data{shape, result_values_we_want};
        Array<Element, rank> result_we_want{lue::create_partitioned_array<Element>(shape, shape)};
        result_we_want.partitions()(0, 0).wait();
        result_we_want.partitions()(0, 0).set_data(std::move(result_we_want_data)).wait();

        lue::test::check_arrays_are_equal(result_we_got, result_we_want);
    }


    template<typename ConditionElement, typename Element, lue::Rank rank>
    void test_where(
        lue::ShapeT<Array<Element, rank>> const& shape,
        std::initializer_list<ConditionElement>&& condition_values,
        Element const true_value,
        Element const false_value,
        std::initializer_list<Element>&& result_values_we_want)
    {
        PartitionData<ConditionElement, rank> condition_data{shape, condition_values};
        Array<ConditionElement, rank> condition{
            lue::create_partitioned_array<ConditionElement>(shape, shape)};
        condition.partitions()(0, 0).wait();
        condition.partitions()(0, 0).set_data(std::move(condition_data)).wait();

        auto result_we_got = lue::value_policies::where(condition, true_value, false_value);

        PartitionData<Element, rank> result_we_want_data{shape, result_values_we_want};
        Array<Element, rank> result_we_want{lue::create_partitioned_array<Element>(shape, shape)};
        result_we_want.partitions()(0, 0).wait();
        result_we_want.partitions()(0, 0).set_data(std::move(result_we_want_data)).wait();

        lue::test::check_arrays_are_equal(result_we_got, result_we_want);
    }

}  // Anonymous namespace


BOOST_AUTO_TEST_CASE(use_case_1)
{
    // Array<Element> = where(Array<std::uint8_t>, Array<Element>, Element)

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

    PartitionData<std::uint8_t, rank> condition_data{
        shape,
        std::initializer_list<std::uint8_t>{
            1,
            0,
            0,
            1,
            1,
            0,
        }};
    Array<std::uint8_t, rank> condition{lue::create_partitioned_array<std::uint8_t>(shape, shape)};
    condition.partitions()(0, 0).wait();
    condition.partitions()(0, 0).set_data(std::move(condition_data)).wait();

    PartitionData<Element, rank> true_data{
        shape,
        std::initializer_list<Element>{
            1,
            2,
            3,
            4,
            5,
            6,
        }};
    Array<Element, rank> true_array{lue::create_partitioned_array<Element>(shape, shape)};
    true_array.partitions()(0, 0).wait();
    true_array.partitions()(0, 0).set_data(std::move(true_data)).wait();

    Element const false_value{9};

    auto result_we_got = lue::default_policies::where(condition, true_array, false_value);

    PartitionData<Element, rank> result_we_want_data{
        shape,
        std::initializer_list<Element>{
            1,
            9,
            9,
            4,
            5,
            9,
        }};
    Array<Element, rank> result_we_want{lue::create_partitioned_array<Element>(shape, shape)};
    result_we_want.partitions()(0, 0).wait();
    result_we_want.partitions()(0, 0).set_data(std::move(result_we_want_data)).wait();

    lue::test::check_arrays_are_equal(result_we_got, result_we_want);
}


BOOST_AUTO_TEST_CASE(use_case_2)
{
    // Array<Element> = where(Array<std::uint8_t>, Array<Element>)

    // Condition:     True array:
    // +-----+-----+  +-----+-----+
    // |  nd |  F  |  |  1  |  nd |
    // +-----+-----+  +-----+-----+
    // |  F  |  T  |  |  3  |  4  |
    // +-----+-----+  +-----+-----+
    // |  T  |  nd |  |  5  |  6  |
    // +-----+-----+  +-----+-----+
    lue::Count const nr_rows{3};
    lue::Count const nr_cols{2};

    using ConditionElement = std::uint8_t;

    using Element = std::int32_t;
    lue::Rank const rank{2};

    ConditionElement ind1;
    Element ind2;
    Element ond;

    {
        using ONDP1 = lue::policy::DefaultOutputNoDataPolicy<ConditionElement>;
        using ONDP2 = lue::policy::DefaultOutputNoDataPolicy<Element>;
        ONDP1 ondp1{};
        ONDP2 ondp2{};

        ondp1.mark_no_data(ind1);
        ondp2.mark_no_data(ind2);
        ondp2.mark_no_data(ond);
    }

    using Shape = lue::ShapeT<Array<Element, rank>>;

    Shape shape{nr_rows, nr_cols};

    PartitionData<ConditionElement, rank> condition_data{
        shape,
        std::initializer_list<ConditionElement>{
            ind1,
            0,
            0,
            1,
            1,
            ind1,
        }};
    Array<ConditionElement, rank> condition{lue::create_partitioned_array<ConditionElement>(shape, shape)};
    condition.partitions()(0, 0).wait();
    condition.partitions()(0, 0).set_data(std::move(condition_data)).wait();

    PartitionData<Element, rank> true_data{
        shape,
        std::initializer_list<Element>{
            1,
            ind2,
            3,
            4,
            5,
            6,
        }};
    Array<Element, rank> true_array{lue::create_partitioned_array<Element>(shape, shape)};
    true_array.partitions()(0, 0).wait();
    true_array.partitions()(0, 0).set_data(std::move(true_data)).wait();

    auto result_we_got = lue::value_policies::where(condition, true_array);

    PartitionData<Element, rank> result_we_want_data{
        shape,
        std::initializer_list<Element>{
            ond,
            ond,
            ond,
            4,
            5,
            ond,
        }};
    Array<Element, rank> result_we_want{lue::create_partitioned_array<Element>(shape, shape)};
    result_we_want.partitions()(0, 0).wait();
    result_we_want.partitions()(0, 0).set_data(std::move(result_we_want_data)).wait();

    lue::test::check_arrays_are_equal(result_we_got, result_we_want);
}


BOOST_AUTO_TEST_CASE(where_array_array)
{
    lue::Rank const rank{2};
    lue::Count const nr_rows{3};
    lue::Count const nr_cols{2};

    using ConditionElement = std::uint8_t;
    using Element = std::int32_t;

    ConditionElement const xc{lue::policy::no_data_value<ConditionElement>};
    Element const xe{lue::policy::no_data_value<Element>};

    // No-data is generated at:
    // - No-data in condition expression
    // - False in condition expression
    // - No-data in true expression

    test_where<ConditionElement, Element, rank>(
        {nr_rows, nr_cols},
        std::initializer_list<ConditionElement>{
            xc,
            0,
            1,
            1,
            1,
            1,
        },
        std::initializer_list<Element>{
            1,
            2,
            xe,
            4,
            5,
            6,
        },
        std::initializer_list<Element>{
            xe,
            xe,
            xe,
            4,
            5,
            6,
        });
}


BOOST_AUTO_TEST_CASE(where_array_value)
{
    lue::Rank const rank{2};
    lue::Count const nr_rows{3};
    lue::Count const nr_cols{2};

    using ConditionElement = std::uint8_t;
    using Element = std::int32_t;

    ConditionElement const xc{lue::policy::no_data_value<ConditionElement>};
    Element const xe{lue::policy::no_data_value<Element>};

    // No-data is generated at:
    // - No-data in condition expression
    // - False in condition expression
    // - No-data in true expression

    test_where<ConditionElement, Element, rank>(
        {nr_rows, nr_cols},
        std::initializer_list<ConditionElement>{
            xc,
            0,
            1,
            1,
            1,
            1,
        },
        5,
        std::initializer_list<Element>{
            xe,
            xe,
            5,
            5,
            5,
            5,
        });

    test_where<ConditionElement, Element, rank>(
        {nr_rows, nr_cols},
        std::initializer_list<ConditionElement>{
            xc,
            0,
            1,
            1,
            1,
            1,
        },
        xe,
        std::initializer_list<Element>{
            xe,
            xe,
            xe,
            xe,
            xe,
            xe,
        });
}


BOOST_AUTO_TEST_CASE(where_array_array_array)
{
    lue::Rank const rank{2};
    lue::Count const nr_rows{3};
    lue::Count const nr_cols{2};

    using ConditionElement = std::uint8_t;
    using Element = std::int32_t;

    ConditionElement const xc{lue::policy::no_data_value<ConditionElement>};
    Element const xe{lue::policy::no_data_value<Element>};

    // No-data is generated at:
    // - No-data in condition expression
    // - No-data in true expression
    // - No-data in false expression

    // No-data is *not* generated at:
    // - True in condition and no-data in false expression
    // - False in condition and no-data in true expression

    test_where<ConditionElement, Element, rank>(
        {nr_rows, nr_cols},
        std::initializer_list<ConditionElement>{
            xc,
            0,
            1,
            1,
            1,
            0,
        },
        std::initializer_list<Element>{
            11,
            12,
            13,
            xe,
            15,
            xe,
        },
        std::initializer_list<Element>{
            21,
            22,
            23,
            24,
            xe,
            26,
        },
        std::initializer_list<Element>{
            xe,
            22,
            13,
            xe,
            15,
            26,
        });
}


BOOST_AUTO_TEST_CASE(where_array_array_value)
{
    lue::Rank const rank{2};
    lue::Count const nr_rows{3};
    lue::Count const nr_cols{2};

    using ConditionElement = std::uint8_t;
    using Element = std::int32_t;

    ConditionElement const xc{lue::policy::no_data_value<ConditionElement>};
    Element const xe{lue::policy::no_data_value<Element>};

    // No-data is generated at:
    // - No-data in condition expression
    // - No-data in true expression
    // - No-data in false expression

    // No-data is *not* generated at:
    // - True in condition and no-data in false expression
    // - False in condition and no-data in true expression

    test_where<ConditionElement, Element, rank>(
        {nr_rows, nr_cols},
        std::initializer_list<ConditionElement>{
            xc,
            0,
            1,
            1,
            1,
            0,
        },
        std::initializer_list<Element>{
            11,
            12,
            13,
            xe,
            15,
            xe,
        },
        5,
        std::initializer_list<Element>{
            xe,
            5,
            13,
            xe,
            15,
            5,
        });

    test_where<ConditionElement, Element, rank>(
        {nr_rows, nr_cols},
        std::initializer_list<ConditionElement>{
            xc,
            0,
            1,
            1,
            1,
            0,
        },
        std::initializer_list<Element>{
            11,
            12,
            13,
            xe,
            15,
            xe,
        },
        xe,
        std::initializer_list<Element>{
            xe,
            xe,
            13,
            xe,
            15,
            xe,
        });
}


BOOST_AUTO_TEST_CASE(where_array_value_array)
{
    lue::Rank const rank{2};
    lue::Count const nr_rows{3};
    lue::Count const nr_cols{2};

    using ConditionElement = std::uint8_t;
    using Element = std::int32_t;

    ConditionElement const xc{lue::policy::no_data_value<ConditionElement>};
    Element const xe{lue::policy::no_data_value<Element>};

    // No-data is generated at:
    // - No-data in condition expression
    // - No-data in true expression
    // - No-data in false expression

    // No-data is *not* generated at:
    // - True in condition and no-data in false expression
    // - False in condition and no-data in true expression

    test_where<ConditionElement, Element, rank>(
        {nr_rows, nr_cols},
        std::initializer_list<ConditionElement>{
            xc,
            0,
            1,
            1,
            1,
            0,
        },
        5,
        std::initializer_list<Element>{
            21,
            22,
            23,
            24,
            xe,
            xe,
        },
        std::initializer_list<Element>{
            xe,
            22,
            5,
            5,
            5,
            xe,
        });

    test_where<ConditionElement, Element, rank>(
        {nr_rows, nr_cols},
        std::initializer_list<ConditionElement>{
            xc,
            0,
            1,
            1,
            1,
            0,
        },
        xe,
        std::initializer_list<Element>{
            21,
            22,
            23,
            24,
            xe,
            xe,
        },
        std::initializer_list<Element>{
            xe,
            22,
            xe,
            xe,
            xe,
            xe,
        });
}


BOOST_AUTO_TEST_CASE(where_array_value_value)
{
    lue::Rank const rank{2};
    lue::Count const nr_rows{3};
    lue::Count const nr_cols{2};

    using ConditionElement = std::uint8_t;
    using Element = std::int32_t;

    ConditionElement const xc{lue::policy::no_data_value<ConditionElement>};
    Element const xe{lue::policy::no_data_value<Element>};

    // No-data is generated at:
    // - No-data in condition expression
    // - No-data in true expression
    // - No-data in false expression

    // No-data is *not* generated at:
    // - True in condition and no-data in false expression
    // - False in condition and no-data in true expression

    test_where<ConditionElement, Element, rank>(
        {nr_rows, nr_cols},
        std::initializer_list<ConditionElement>{
            xc,
            0,
            1,
            1,
            1,
            0,
        },
        5,
        6,
        std::initializer_list<Element>{
            xe,
            6,
            5,
            5,
            5,
            6,
        });

    test_where<ConditionElement, Element, rank>(
        {nr_rows, nr_cols},
        std::initializer_list<ConditionElement>{
            xc,
            0,
            1,
            1,
            1,
            0,
        },
        xe,
        6,
        std::initializer_list<Element>{
            xe,
            6,
            xe,
            xe,
            xe,
            6,
        });

    test_where<ConditionElement, Element, rank>(
        {nr_rows, nr_cols},
        std::initializer_list<ConditionElement>{
            xc,
            0,
            1,
            1,
            1,
            0,
        },
        5,
        xe,
        std::initializer_list<Element>{
            xe,
            xe,
            5,
            5,
            5,
            xe,
        });
}
