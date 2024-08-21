#pragma once
#include "lue/framework/algorithm/policy/policy_traits.hpp"
#include "lue/framework/algorithm/scalar.hpp"
#include "lue/framework/partitioned_array.hpp"


namespace lue {

    // local_operation(array)
    template<typename Policies, Rank rank, typename Functor>
    auto unary_local_operation(
        Policies const& policies,
        PartitionedArray<policy::InputElementT<Policies, 0>, rank> const& input_array,
        Functor const& functor) -> PartitionedArray<policy::OutputElementT<Policies, 0>, rank>;


    // local_operation(scalar)
    template<typename Policies, typename Functor>
    auto unary_local_operation(
        Policies const& policies,
        Scalar<policy::InputElementT<Policies, 0>> const& input_scalar,
        Functor const& functor) -> Scalar<policy::OutputElementT<Policies, 0>>;

}  // namespace lue


// All overloads *with* a Policies template parameter
#define LUE_UNARY_LOCAL_OPERATION_OVERLOADS_WITH_POLICIES_DIFFERENT_OUTPUT_ELEMENT(name, Functor)            \
                                                                                                             \
    /* f(policies, array) */                                                                                 \
    template<typename Policies, Rank rank>                                                                   \
    auto name(                                                                                               \
        Policies const& policies, PartitionedArray<policy::InputElementT<Policies, 0>, rank> const& array)   \
        ->PartitionedArray<policy::OutputElementT<Policies, 0>, rank>                                        \
    {                                                                                                        \
        return unary_local_operation(                                                                        \
            policies,                                                                                        \
            array,                                                                                           \
            Functor<policy::InputElementT<Policies, 0>, policy::OutputElementT<Policies, 0>>{});             \
    }                                                                                                        \
                                                                                                             \
                                                                                                             \
    /* f(policies, scalar) */                                                                                \
    template<typename Policies>                                                                              \
    auto name(Policies const& policies, Scalar<policy::InputElementT<Policies, 0>> const& scalar)            \
        ->Scalar<policy::OutputElementT<Policies, 0>>                                                        \
    {                                                                                                        \
        return unary_local_operation(                                                                        \
            policies,                                                                                        \
            scalar,                                                                                          \
            Functor<policy::InputElementT<Policies, 0>, policy::OutputElementT<Policies, 0>>{});             \
    }


// All overloads *with* a Policies template parameter
#define LUE_UNARY_LOCAL_OPERATION_OVERLOADS_WITH_POLICIES_SAME_OUTPUT_ELEMENT(name, Functor)                 \
                                                                                                             \
    /* f(policies, array) */                                                                                 \
    template<typename Policies, Rank rank>                                                                   \
    auto name(                                                                                               \
        Policies const& policies, PartitionedArray<policy::InputElementT<Policies, 0>, rank> const& array)   \
        ->PartitionedArray<policy::OutputElementT<Policies, 0>, rank>                                        \
    {                                                                                                        \
        return unary_local_operation(policies, array, Functor<policy::InputElementT<Policies, 0>>{});        \
    }                                                                                                        \
                                                                                                             \
                                                                                                             \
    /* f(policies, scalar) */                                                                                \
    template<typename Policies>                                                                              \
    auto name(Policies const& policies, Scalar<policy::InputElementT<Policies, 0>> const& scalar)            \
        ->Scalar<policy::OutputElementT<Policies, 0>>                                                        \
    {                                                                                                        \
        return unary_local_operation(policies, scalar, Functor<policy::InputElementT<Policies, 0>>{});       \
    }


// All overloads *without* a Policies template parameter
#define LUE_UNARY_LOCAL_OPERATION_OVERLOADS_WITHOUT_POLICIES_DIFFERENT_OUTPUT_ELEMENT(name, Policies)        \
                                                                                                             \
    /* f(array) */                                                                                           \
    template<typename OutputElement, typename InputElement, Rank rank>                                       \
    auto name(PartitionedArray<InputElement, rank> const& array)->PartitionedArray<OutputElement, rank>      \
    {                                                                                                        \
        using Policies_ = Policies<OutputElement, InputElement>;                                             \
                                                                                                             \
        return name(Policies_{}, array);                                                                     \
    }                                                                                                        \
                                                                                                             \
                                                                                                             \
    /* f(scalar) */                                                                                          \
    template<typename OutputElement, typename InputElement>                                                  \
    auto name(Scalar<InputElement> const& scalar)->Scalar<OutputElement>                                     \
    {                                                                                                        \
        using Policies_ = Policies<OutputElement, InputElement>;                                             \
                                                                                                             \
        return name(Policies_{}, scalar);                                                                    \
    }


// All overloads *without* a Policies template parameter
#define LUE_UNARY_LOCAL_OPERATION_OVERLOADS_WITHOUT_POLICIES_SAME_OUTPUT_ELEMENT(name, Policies)             \
                                                                                                             \
    /* f(array) */                                                                                           \
    template<typename Element, Rank rank>                                                                    \
    auto name(PartitionedArray<Element, rank> const& array)->PartitionedArray<Element, rank>                 \
    {                                                                                                        \
        using Policies_ = Policies<Element>;                                                                 \
                                                                                                             \
        return name(Policies_{}, array);                                                                     \
    }                                                                                                        \
                                                                                                             \
                                                                                                             \
    /* f(scalar) */                                                                                          \
    template<typename Element>                                                                               \
    auto name(Scalar<Element> const& scalar)->Scalar<Element>                                                \
    {                                                                                                        \
        using Policies_ = Policies<Element>;                                                                 \
                                                                                                             \
        return name(Policies_{}, scalar);                                                                    \
    }
