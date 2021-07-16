#define BOOST_TEST_MODULE lue framework algorithm where
#include "lue/framework/algorithm/create_partitioned_array.hpp"
#include "lue/framework/algorithm/default_policies/where.hpp"
#include "lue/framework/algorithm/definition/where.hpp"
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
    Array<bool, rank> condition{lue::create_partitioned_array<bool>(shape, shape)};
    condition.partitions()(0, 0).wait();
    condition.partitions()(0, 0).set_data(std::move(condition_data));

    PartitionData<Element, rank> true_data{
            shape,
            std::initializer_list<Element>{
                1, 2,
                3, 4,
                5, 6,
            }
        };
    Array<Element, rank> true_array{lue::create_partitioned_array<Element>(shape, shape)};
    true_array.partitions()(0, 0).wait();
    true_array.partitions()(0, 0).set_data(std::move(true_data));

    Element const false_value{9};

    auto result_we_got = lue::default_policies::where(condition, true_array, false_value);

    PartitionData<Element, rank> result_we_want_data{
            shape,
            std::initializer_list<Element>{
                1, 9,
                9, 4,
                5, 9,
            }
        };
    Array<Element, rank> result_we_want{lue::create_partitioned_array<Element>(shape, shape)};
    result_we_want.partitions()(0, 0).wait();
    result_we_want.partitions()(0, 0).set_data(std::move(result_we_want_data));

    lue::test::check_arrays_are_equal(result_we_got, result_we_want);
}


BOOST_AUTO_TEST_CASE(use_case_2)
{
    // Array<Element> = where(Array<bool>, Array<Element>)

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

    ConditionElement const ind1{66};
    Element const ind2{77};
    Element const ond{99};

    using Shape = lue::ShapeT<Array<Element, rank>>;

    Shape shape{nr_rows, nr_cols};

    PartitionData<ConditionElement, rank> condition_data{
            shape,
            std::initializer_list<ConditionElement>{
                ind1, 0,
                   0, 1,
                   1, ind1,
            }
        };
    Array<ConditionElement, rank> condition{lue::create_partitioned_array<ConditionElement>(shape, shape)};
    condition.partitions()(0, 0).wait();
    condition.partitions()(0, 0).set_data(std::move(condition_data));

    PartitionData<Element, rank> true_data{
            shape,
            std::initializer_list<Element>{
                1, ind2,
                3, 4,
                5, 6,
            }
        };
    Array<Element, rank> true_array{lue::create_partitioned_array<Element>(shape, shape)};
    true_array.partitions()(0, 0).wait();
    true_array.partitions()(0, 0).set_data(std::move(true_data));

    using DomainPolicy = lue::policy::where::DomainPolicy<ConditionElement, Element>;
    using OutputPolicies =
        lue::policy::OutputPolicies<lue::policy::MarkNoDataByValue<Element>>;
    using InputExpressionPolicies =
        lue::policy::InputPolicies<lue::policy::DetectNoDataByValue<Element>>;
    using InputConditionPolicies =
        lue::policy::InputPolicies<lue::policy::DetectNoDataByValue<ConditionElement>>;

    using Policies = lue::policy::Policies<
        DomainPolicy,
        lue::policy::OutputsPolicies<OutputPolicies>,
        lue::policy::InputsPolicies<InputConditionPolicies, InputExpressionPolicies>>;

    OutputPolicies output_policies{
        lue::policy::MarkNoDataByValue<Element>{ond}};
    InputConditionPolicies condition_policies{
        lue::policy::DetectNoDataByValue<ConditionElement>{ind1}};
    InputExpressionPolicies expression_policies{
        lue::policy::DetectNoDataByValue<Element>{ind2}};

    Policies policies{DomainPolicy{}, output_policies, condition_policies, expression_policies};

    auto result_we_got = lue::where(policies, condition, true_array);

    PartitionData<Element, rank> result_we_want_data{
            shape,
            std::initializer_list<Element>{
                ond, ond,
                ond,   4,
                  5, ond,
            }
        };
    Array<Element, rank> result_we_want{lue::create_partitioned_array<Element>(shape, shape)};
    result_we_want.partitions()(0, 0).wait();
    result_we_want.partitions()(0, 0).set_data(std::move(result_we_want_data));

    lue::test::check_arrays_are_equal(result_we_got, result_we_want);
}
