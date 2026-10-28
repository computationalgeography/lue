#pragma once
#include "lue/framework/algorithm/policy/policy_traits.hpp"
#include "lue/framework/algorithm/scalar.hpp"
#include "lue/framework/partitioned_array_decl.hpp"
#include <type_traits>


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
#define LUE_UNARY_LOCAL_OPERATION_OVERLOADS_WITH_POLICIES_DIFFERENT_OUTPUT_ELEMENT(                          \
    name, Functor, InputElementConcept, OutputElementConcept)                                                \
                                                                                                             \
    /* f(policies, array) */                                                                                 \
    template<typename Policies, Rank rank>                                                                   \
        requires(InputElementConcept<policy::InputElementT<Policies, 0>> &&                                  \
                 OutputElementConcept<policy::OutputElementT<Policies, 0>>)                                  \
    auto name(                                                                                               \
        Policies const& policies, PartitionedArray<policy::InputElementT<Policies, 0>, rank> const& array)   \
        -> PartitionedArray<policy::OutputElementT<Policies, 0>, rank>                                       \
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
        requires(InputElementConcept<policy::InputElementT<Policies, 0>> &&                                  \
                 OutputElementConcept<policy::OutputElementT<Policies, 0>>)                                  \
    auto name(Policies const& policies, Scalar<policy::InputElementT<Policies, 0>> const& scalar)            \
        -> Scalar<policy::OutputElementT<Policies, 0>>                                                       \
    {                                                                                                        \
        return unary_local_operation(                                                                        \
            policies,                                                                                        \
            scalar,                                                                                          \
            Functor<policy::InputElementT<Policies, 0>, policy::OutputElementT<Policies, 0>>{});             \
    }                                                                                                        \
                                                                                                             \
                                                                                                             \
    /* f(policies, value) */                                                                                 \
    template<typename Policies>                                                                              \
        requires(InputElementConcept<policy::InputElementT<Policies, 0>> &&                                  \
                 OutputElementConcept<policy::OutputElementT<Policies, 0>>)                                  \
    auto name(Policies const& policies, policy::InputElementT<Policies, 0> const& value)                     \
        -> Scalar<policy::OutputElementT<Policies, 0>>                                                       \
    {                                                                                                        \
        return name(policies, Scalar{value});                                                                \
    }


// All overloads *with* a Policies template parameter
#define LUE_UNARY_LOCAL_OPERATION_OVERLOADS_WITH_POLICIES_SAME_OUTPUT_ELEMENT(name, Functor, ElementConcept) \
                                                                                                             \
    /* f(policies, array) */                                                                                 \
    template<typename Policies, Rank rank>                                                                   \
        requires(ElementConcept<policy::InputElementT<Policies, 0>> &&                                       \
                 ElementConcept<policy::OutputElementT<Policies, 0>> &&                                      \
                 std::same_as<policy::InputElementT<Policies, 0>, policy::OutputElementT<Policies, 0>>)      \
    auto name(                                                                                               \
        Policies const& policies, PartitionedArray<policy::InputElementT<Policies, 0>, rank> const& array)   \
        -> PartitionedArray<policy::OutputElementT<Policies, 0>, rank>                                       \
    {                                                                                                        \
        return unary_local_operation(policies, array, Functor<policy::InputElementT<Policies, 0>>{});        \
    }                                                                                                        \
                                                                                                             \
                                                                                                             \
    /* f(policies, scalar) */                                                                                \
    template<typename Policies>                                                                              \
        requires(ElementConcept<policy::InputElementT<Policies, 0>> &&                                       \
                 ElementConcept<policy::OutputElementT<Policies, 0>> &&                                      \
                 std::same_as<policy::InputElementT<Policies, 0>, policy::OutputElementT<Policies, 0>>)      \
    auto name(Policies const& policies, Scalar<policy::InputElementT<Policies, 0>> const& scalar)            \
        -> Scalar<policy::OutputElementT<Policies, 0>>                                                       \
    {                                                                                                        \
        return unary_local_operation(policies, scalar, Functor<policy::InputElementT<Policies, 0>>{});       \
    }                                                                                                        \
                                                                                                             \
                                                                                                             \
    /* f(policies, value) */                                                                                 \
    template<typename Policies>                                                                              \
        requires(ElementConcept<policy::InputElementT<Policies, 0>> &&                                       \
                 ElementConcept<policy::OutputElementT<Policies, 0>> &&                                      \
                 std::same_as<policy::InputElementT<Policies, 0>, policy::OutputElementT<Policies, 0>>)      \
    auto name(Policies const& policies, policy::InputElementT<Policies, 0> const& value)                     \
        -> Scalar<policy::OutputElementT<Policies, 0>>                                                       \
    {                                                                                                        \
        return name(policies, Scalar{value});                                                                \
    }


// All overloads *without* a Policies template parameter
// Create a Policies instance and call the general implementation
#define LUE_UNARY_LOCAL_OPERATION_OVERLOADS_WITHOUT_POLICIES_DIFFERENT_OUTPUT_ELEMENT(                       \
    name, Policies, InputElementConcept, OutputElementConcept)                                               \
                                                                                                             \
    /* f(array) */                                                                                           \
    template<OutputElementConcept OutputElement, InputElementConcept InputElement, Rank rank>                \
    auto name(PartitionedArray<InputElement, rank> const& array) -> PartitionedArray<OutputElement, rank>    \
    {                                                                                                        \
        using Policies_ = Policies<OutputElement, InputElement>;                                             \
                                                                                                             \
        return name(Policies_{}, array);                                                                     \
    }                                                                                                        \
                                                                                                             \
                                                                                                             \
    /* f(scalar) */                                                                                          \
    template<OutputElementConcept OutputElement, InputElementConcept InputElement>                           \
    auto name(Scalar<InputElement> const& scalar) -> Scalar<OutputElement>                                   \
    {                                                                                                        \
        using Policies_ = Policies<OutputElement, InputElement>;                                             \
                                                                                                             \
        return name(Policies_{}, scalar);                                                                    \
    }                                                                                                        \
                                                                                                             \
                                                                                                             \
    /* f(value) */                                                                                           \
    template<OutputElementConcept OutputElement, InputElementConcept InputElement>                           \
    auto name(InputElement const& value) -> Scalar<OutputElement>                                            \
    {                                                                                                        \
        using Policies_ = Policies<OutputElement, InputElement>;                                             \
                                                                                                             \
        return lue::name(Policies_{}, value);                                                                \
    }


// All overloads *without* a Policies template parameter
// Create a Policies instance and call the general implementation
#define LUE_UNARY_LOCAL_OPERATION_OVERLOADS_WITHOUT_POLICIES_SAME_OUTPUT_ELEMENT(                            \
    name, Policies, ElementConcept)                                                                          \
                                                                                                             \
    /* f(array) */                                                                                           \
    template<ElementConcept Element, Rank rank>                                                              \
    auto name(PartitionedArray<Element, rank> const& array) -> PartitionedArray<Element, rank>               \
    {                                                                                                        \
        using Policies_ = Policies<Element>;                                                                 \
                                                                                                             \
        return name(Policies_{}, array);                                                                     \
    }                                                                                                        \
                                                                                                             \
                                                                                                             \
    /* f(scalar) */                                                                                          \
    template<ElementConcept Element>                                                                         \
    auto name(Scalar<Element> const& scalar) -> Scalar<Element>                                              \
    {                                                                                                        \
        using Policies_ = Policies<Element>;                                                                 \
                                                                                                             \
        return name(Policies_{}, scalar);                                                                    \
    }                                                                                                        \
                                                                                                             \
                                                                                                             \
    /* f(value) */                                                                                           \
    template<ElementConcept Element>                                                                         \
    auto name(Element const& value) -> Scalar<Element>                                                       \
    {                                                                                                        \
        using Policies_ = Policies<Element>;                                                                 \
                                                                                                             \
        return lue::name(Policies_{}, value);                                                                \
    }
