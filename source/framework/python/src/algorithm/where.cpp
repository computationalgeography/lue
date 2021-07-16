#include "lue/framework/algorithm/value_policies/where.hpp"
#include <pybind11/pybind11.h>


namespace lue::framework {
    namespace {

        template<
            typename Condition,
            Rank rank,
            typename... Expression>
        class Where
        {
        };


        // where(condition_array, true_array)
        template<
            typename ConditionElement,
            Rank rank,
            typename ExpressionElement>
        class Where<
            PartitionedArray<ConditionElement, rank>,
            rank,
            PartitionedArray<ExpressionElement, rank>>
        {

            public:

                using OutputElement = ExpressionElement;

                static PartitionedArray<ExpressionElement, rank> operation(
                    PartitionedArray<ConditionElement, rank> const& condition_array,
                    PartitionedArray<ExpressionElement, rank> const& true_array)
                {
                    return lue::value_policies::where(condition_array, true_array);
                }

        };


        // where(condition_array, true_value_f)
        template<
            typename ConditionElement,
            Rank rank,
            typename ExpressionElement>
        class Where<
            PartitionedArray<ConditionElement, rank>,
            rank,
            hpx::shared_future<ExpressionElement>>
        {

            public:

                using OutputElement = ExpressionElement;

                static PartitionedArray<ExpressionElement, rank> operation(
                    PartitionedArray<ConditionElement, rank> const& condition_array,
                    hpx::shared_future<ExpressionElement> const& true_value_f)
                {
                    return lue::value_policies::where(condition_array, true_value_f);
                }

        };


        // where(condition_array, true_value)
        template<
            typename ConditionElement,
            Rank rank,
            typename ExpressionElement>
        class Where<
            PartitionedArray<ConditionElement, rank>,
            rank,
            ExpressionElement>
        {

            public:

                using OutputElement = ExpressionElement;

                static PartitionedArray<ExpressionElement, rank> operation(
                    PartitionedArray<ConditionElement, rank> const& condition_array,
                    ExpressionElement const& true_value)
                {
                    return lue::value_policies::where(condition_array, true_value);
                }

        };


        // where(condition_array, true_array, false_array)
        template<
            typename ConditionElement,
            Rank rank,
            typename ExpressionElement>
        class Where<
            PartitionedArray<ConditionElement, rank>,
            rank,
            PartitionedArray<ExpressionElement, rank>,
            PartitionedArray<ExpressionElement, rank>>
        {

            public:

                using OutputElement = ExpressionElement;

                static PartitionedArray<ExpressionElement, rank> operation(
                    PartitionedArray<ConditionElement, rank> const& condition_array,
                    PartitionedArray<ExpressionElement, rank> const& true_array,
                    PartitionedArray<ExpressionElement, rank> const& false_array)
                {
                    return lue::value_policies::where(condition_array, true_array, false_array);
                }

        };


        // where(condition_array, true_array, false_value_f)
        template<
            typename ConditionElement,
            Rank rank,
            typename ExpressionElement>
        class Where<
            PartitionedArray<ConditionElement, rank>,
            rank,
            PartitionedArray<ExpressionElement, rank>,
            hpx::shared_future<ExpressionElement>>
        {

            public:

                using OutputElement = ExpressionElement;

                static PartitionedArray<ExpressionElement, rank> operation(
                    PartitionedArray<ConditionElement, rank> const& condition_array,
                    PartitionedArray<ExpressionElement, rank> const& true_array,
                    hpx::shared_future<ExpressionElement> const& false_value_f)
                {
                    return lue::value_policies::where(condition_array, true_array, false_value_f);
                }

        };


        // where(condition_array, true_array, false_value)
        template<
            typename ConditionElement,
            Rank rank,
            typename ExpressionElement>
        class Where<
            PartitionedArray<ConditionElement, rank>,
            rank,
            PartitionedArray<ExpressionElement, rank>,
            ExpressionElement>
        {

            public:

                using OutputElement = ExpressionElement;

                static PartitionedArray<ExpressionElement, rank> operation(
                    PartitionedArray<ConditionElement, rank> const& condition_array,
                    PartitionedArray<ExpressionElement, rank> const& true_array,
                    ExpressionElement const& false_value)
                {
                    return lue::value_policies::where(condition_array, true_array, false_value);
                }

        };


        // where(condition_array, true_value_f, false_array)
        template<
            typename ConditionElement,
            Rank rank,
            typename ExpressionElement>
        class Where<
            PartitionedArray<ConditionElement, rank>,
            rank,
            hpx::shared_future<ExpressionElement>,
            PartitionedArray<ExpressionElement, rank>>
        {

            public:

                using OutputElement = ExpressionElement;

                static PartitionedArray<ExpressionElement, rank> operation(
                    PartitionedArray<ConditionElement, rank> const& condition_array,
                    hpx::shared_future<ExpressionElement> const& true_value_f,
                    PartitionedArray<ExpressionElement, rank> const& false_array)
                {
                    return lue::value_policies::where(condition_array, true_value_f, false_array);
                }

        };


        // where(condition_array, true_value, false_array)
        template<
            typename ConditionElement,
            Rank rank,
            typename ExpressionElement>
        class Where<
            PartitionedArray<ConditionElement, rank>,
            rank,
            ExpressionElement,
            PartitionedArray<ExpressionElement, rank>>
        {

            public:

                using OutputElement = ExpressionElement;

                static PartitionedArray<ExpressionElement, rank> operation(
                    PartitionedArray<ConditionElement, rank> const& condition_array,
                    ExpressionElement const& true_value,
                    PartitionedArray<ExpressionElement, rank> const& false_array)
                {
                    return lue::value_policies::where(condition_array, true_value, false_array);
                }

        };


        // where(condition_array, true_value_f, false_value_f)
        template<
            typename ConditionElement,
            Rank rank,
            typename ExpressionElement>
        class Where<
            PartitionedArray<ConditionElement, rank>,
            rank,
            hpx::shared_future<ExpressionElement>,
            hpx::shared_future<ExpressionElement>>
        {

            public:

                using OutputElement = ExpressionElement;

                static PartitionedArray<ExpressionElement, rank> operation(
                    PartitionedArray<ConditionElement, rank> const& condition_array,
                    hpx::shared_future<ExpressionElement> const& true_value_f,
                    hpx::shared_future<ExpressionElement> const& false_value_f)
                {
                    return lue::value_policies::where(condition_array, true_value_f, false_value_f);
                }

        };


        // where(condition_array, true_value, false_value)
        template<
            typename ConditionElement,
            Rank rank,
            typename ExpressionElement>
        class Where<
            PartitionedArray<ConditionElement, rank>,
            rank,
            ExpressionElement,
            ExpressionElement>
        {

            public:

                using OutputElement = ExpressionElement;

                static PartitionedArray<ExpressionElement, rank> operation(
                    PartitionedArray<ConditionElement, rank> const& condition_array,
                    ExpressionElement const& true_value,
                    ExpressionElement const& false_value)
                {
                    return lue::value_policies::where(condition_array, true_value, false_value);
                }

        };


        template<
            typename ConditionElement,
            Rank rank,
            typename... Expression>
        PartitionedArray<
            typename Where<PartitionedArray<ConditionElement, rank>, rank, Expression...>::OutputElement, rank>
                where(
                    PartitionedArray<ConditionElement, rank> const& condition_array,
                    Expression const&... expression)
        {
            using ConditionArray = PartitionedArray<ConditionElement, rank>;

            return Where<ConditionArray, rank, Expression...>::operation(condition_array, expression...);
        }

    }  // Anonymous namespace


#define WHERE_OVERLOADS(Element, rank) \
    module.def("where",  \
        where<std::uint8_t, rank, PartitionedArray<Element, rank>>);  \
    module.def("where",  \
        where<std::uint8_t, rank, hpx::shared_future<Element>>);  \
    module.def("where",  \
        where<std::uint8_t, rank, Element>);  \
    module.def("where",  \
        where<std::uint8_t, rank, PartitionedArray<Element, rank>, PartitionedArray<Element, rank>>);  \
    module.def("where",  \
        where<std::uint8_t, rank, PartitionedArray<Element, rank>, hpx::shared_future<Element>>);  \
    module.def("where",  \
        where<std::uint8_t, rank, PartitionedArray<Element, rank>, Element>);  \
    module.def("where",  \
        where<std::uint8_t, rank, hpx::shared_future<Element>, PartitionedArray<Element, rank>>);  \
    module.def("where",  \
        where<std::uint8_t, rank, Element, PartitionedArray<Element, rank>>);  \
    module.def("where",  \
        where<std::uint8_t, rank, hpx::shared_future<Element>, hpx::shared_future<Element>>);  \
    module.def("where",  \
        where<std::uint8_t, rank, Element, Element>);


    void bind_where(
        pybind11::module& module)
    {
        // TODO How to document these?
        WHERE_OVERLOADS(std::uint8_t, 2);
        WHERE_OVERLOADS(std::uint32_t, 2);
        WHERE_OVERLOADS(std::uint64_t, 2);
        WHERE_OVERLOADS(std::int32_t, 2);
        WHERE_OVERLOADS(std::int64_t, 2);
        WHERE_OVERLOADS(float, 2);
        WHERE_OVERLOADS(double, 2);
    }

}  // namespace lue::framework
