#pragma once
#include "lue/framework/algorithm/policy/policy_traits.hpp"
#include "lue/framework/algorithm/scalar.hpp"
#include "lue/framework/partitioned_array_decl.hpp"
#include <type_traits>


namespace lue {

    // local_operation(partition, scalar)
    template<typename Policies, Rank rank, typename Functor>
    auto binary_local_operation(
        hpx::id_type locality_id,
        Policies const& policies,
        ArrayPartition<policy::InputElementT<Policies, 0>, rank> const& input_partition,
        hpx::shared_future<policy::InputElementT<Policies, 1>> const& input_scalar,
        Functor const& functor) -> ArrayPartition<policy::OutputElementT<Policies, 0>, rank>;

    // local_operation(array, array)
    template<typename Policies, Rank rank, typename Functor>
    auto binary_local_operation(
        Policies const& policies,
        PartitionedArray<policy::InputElementT<Policies, 0>, rank> const& input_array1,
        PartitionedArray<policy::InputElementT<Policies, 1>, rank> const& input_array2,
        Functor const& functor) -> PartitionedArray<policy::OutputElementT<Policies, 0>, rank>;

    // local_operation(array, scalar)
    template<typename Policies, Rank rank, typename Functor>
    auto binary_local_operation(
        Policies const& policies,
        PartitionedArray<policy::InputElementT<Policies, 0>, rank> const& input_array,
        hpx::shared_future<policy::InputElementT<Policies, 1>> const& input_scalar,
        Functor const& functor) -> PartitionedArray<policy::OutputElementT<Policies, 0>, rank>;

    // local_operation(scalar, array)
    template<typename Policies, Rank rank, typename Functor>
    auto binary_local_operation(
        Policies const& policies,
        hpx::shared_future<policy::InputElementT<Policies, 0>> const& input_scalar,
        PartitionedArray<policy::InputElementT<Policies, 1>, rank> const& input_array,
        Functor const& functor) -> PartitionedArray<policy::OutputElementT<Policies, 0>, rank>;

    // local_operation(scalar, scalar)
    template<typename Policies, typename Functor>
    auto binary_local_operation(
        Policies const& policies,
        hpx::shared_future<policy::InputElementT<Policies, 0>> const& input_scalar1,
        hpx::shared_future<policy::InputElementT<Policies, 1>> const& input_scalar2,
        Functor const& functor) -> hpx::future<policy::OutputElementT<Policies, 0>>;

}  // namespace lue


// All overloads *with* a Policies template parameter
#define LUE_BINARY_LOCAL_OPERATION_OVERLOADS_WITH_POLICIES_DIFFERENT_OUTPUT_ELEMENT(name, Functor)           \
                                                                                                             \
    /* f(policies, array, array) */                                                                          \
    template<typename Policies, Rank rank>                                                                   \
    auto name(                                                                                               \
        Policies const& policies,                                                                            \
        PartitionedArray<policy::InputElementT<Policies, 0>, rank> const& array1,                            \
        PartitionedArray<policy::InputElementT<Policies, 1>, rank> const& array2)                            \
        -> PartitionedArray<policy::OutputElementT<Policies, 0>, rank>                                       \
    {                                                                                                        \
        return binary_local_operation(                                                                       \
            policies,                                                                                        \
            array1,                                                                                          \
            array2,                                                                                          \
            Functor<policy::InputElementT<Policies, 0>, policy::OutputElementT<Policies, 0>>{});             \
    }                                                                                                        \
                                                                                                             \
                                                                                                             \
    /* f(policies, array, scalar) */                                                                         \
    template<typename Policies, Rank rank>                                                                   \
    auto name(                                                                                               \
        Policies const& policies,                                                                            \
        PartitionedArray<policy::InputElementT<Policies, 0>, rank> const& array,                             \
        Scalar<policy::InputElementT<Policies, 1>> const& scalar)                                            \
        -> PartitionedArray<policy::OutputElementT<Policies, 0>, rank>                                       \
    {                                                                                                        \
        return binary_local_operation(                                                                       \
            policies,                                                                                        \
            array,                                                                                           \
            scalar.future(),                                                                                 \
            Functor<policy::InputElementT<Policies, 0>, policy::OutputElementT<Policies, 0>>{});             \
    }                                                                                                        \
                                                                                                             \
                                                                                                             \
    /* f(policies, array, value) */                                                                          \
    template<typename Policies, Rank rank>                                                                   \
    auto name(                                                                                               \
        Policies const& policies,                                                                            \
        PartitionedArray<policy::InputElementT<Policies, 0>, rank> const& array,                             \
        policy::InputElementT<Policies, 1> const value)                                                      \
        -> PartitionedArray<policy::OutputElementT<Policies, 0>, rank>                                       \
    {                                                                                                        \
        return name(policies, array, Scalar{value});                                                         \
    }                                                                                                        \
                                                                                                             \
                                                                                                             \
    /* f(policies, scalar, array) */                                                                         \
    template<typename Policies, Rank rank>                                                                   \
    auto name(                                                                                               \
        Policies const& policies,                                                                            \
        Scalar<policy::InputElementT<Policies, 0>> const& scalar,                                            \
        PartitionedArray<policy::InputElementT<Policies, 1>, rank> const& array)                             \
        -> PartitionedArray<policy::OutputElementT<Policies, 0>, rank>                                       \
    {                                                                                                        \
        return binary_local_operation(                                                                       \
            policies,                                                                                        \
            scalar.future(),                                                                                 \
            array,                                                                                           \
            Functor<policy::InputElementT<Policies, 0>, policy::OutputElementT<Policies, 0>>{});             \
    }                                                                                                        \
                                                                                                             \
                                                                                                             \
    /* f(policies, value, array) */                                                                          \
    template<typename Policies, Rank rank>                                                                   \
    auto name(                                                                                               \
        Policies const& policies,                                                                            \
        policy::InputElementT<Policies, 0> const value,                                                      \
        PartitionedArray<policy::InputElementT<Policies, 1>, rank> const& array)                             \
        -> PartitionedArray<policy::OutputElementT<Policies, 0>, rank>                                       \
    {                                                                                                        \
        return name(policies, Scalar{value}, array);                                                         \
    }                                                                                                        \
                                                                                                             \
                                                                                                             \
    /* f(policies, scalar, scalar) */                                                                        \
    template<typename Policies>                                                                              \
    auto name(                                                                                               \
        Policies const& policies,                                                                            \
        Scalar<policy::InputElementT<Policies, 0>> const& scalar1,                                           \
        Scalar<policy::InputElementT<Policies, 1>> const& scalar2)                                           \
        -> Scalar<policy::OutputElementT<Policies, 0>>                                                       \
    {                                                                                                        \
        return binary_local_operation(                                                                       \
            policies,                                                                                        \
            scalar1.future(),                                                                                \
            scalar2.future(),                                                                                \
            Functor<policy::InputElementT<Policies, 0>, policy::OutputElementT<Policies, 0>>{});             \
    }                                                                                                        \
                                                                                                             \
                                                                                                             \
    /* f(policies, scalar, value) */                                                                         \
    template<typename Policies>                                                                              \
    auto name(                                                                                               \
        Policies const& policies,                                                                            \
        Scalar<policy::InputElementT<Policies, 0>> const& scalar,                                            \
        policy::InputElementT<Policies, 1> const value) -> Scalar<policy::OutputElementT<Policies, 0>>       \
    {                                                                                                        \
        return name(policies, scalar, Scalar{value});                                                        \
    }                                                                                                        \
                                                                                                             \
                                                                                                             \
    /* f(policies, value, scalar) */                                                                         \
    template<typename Policies>                                                                              \
    auto name(                                                                                               \
        Policies const& policies,                                                                            \
        policy::InputElementT<Policies, 0> const value,                                                      \
        Scalar<policy::InputElementT<Policies, 1>> const& scalar)                                            \
        -> Scalar<policy::OutputElementT<Policies, 0>>                                                       \
    {                                                                                                        \
        return name(policies, Scalar{value}, scalar);                                                        \
    }                                                                                                        \
                                                                                                             \
                                                                                                             \
    /* f(policies, value, value) */                                                                          \
    template<typename Policies>                                                                              \
    auto name(                                                                                               \
        Policies const& policies,                                                                            \
        policy::InputElementT<Policies, 0> const value1,                                                     \
        policy::InputElementT<Policies, 1> const value2) -> Scalar<policy::OutputElementT<Policies, 0>>      \
    {                                                                                                        \
        return name(policies, Scalar{value1}, Scalar{value2});                                               \
    }


// All overloads *with* a Policies template parameter
#define LUE_BINARY_LOCAL_OPERATION_OVERLOADS_WITH_POLICIES_SAME_OUTPUT_ELEMENT(name, Functor)                \
                                                                                                             \
    /* f(policies, array, array) */                                                                          \
    template<typename Policies, Rank rank>                                                                   \
    auto name(                                                                                               \
        Policies const& policies,                                                                            \
        PartitionedArray<policy::InputElementT<Policies, 0>, rank> const& array1,                            \
        PartitionedArray<policy::InputElementT<Policies, 1>, rank> const& array2)                            \
        -> PartitionedArray<policy::OutputElementT<Policies, 0>, rank>                                       \
    {                                                                                                        \
        return binary_local_operation(                                                                       \
            policies, array1, array2, Functor<policy::InputElementT<Policies, 0>>{});                        \
    }                                                                                                        \
                                                                                                             \
                                                                                                             \
    /* f(policies, array, scalar) */                                                                         \
    template<typename Policies, Rank rank>                                                                   \
    auto name(                                                                                               \
        Policies const& policies,                                                                            \
        PartitionedArray<policy::InputElementT<Policies, 0>, rank> const& array,                             \
        Scalar<policy::InputElementT<Policies, 1>> const& scalar)                                            \
        -> PartitionedArray<policy::OutputElementT<Policies, 0>, rank>                                       \
    {                                                                                                        \
        return binary_local_operation(                                                                       \
            policies, array, scalar.future(), Functor<policy::InputElementT<Policies, 0>>{});                \
    }                                                                                                        \
                                                                                                             \
                                                                                                             \
    /* f(policies, array, value) */                                                                          \
    template<typename Policies, Rank rank>                                                                   \
    auto name(                                                                                               \
        Policies const& policies,                                                                            \
        PartitionedArray<policy::InputElementT<Policies, 0>, rank> const& array,                             \
        policy::InputElementT<Policies, 1> const value)                                                      \
        -> PartitionedArray<policy::OutputElementT<Policies, 0>, rank>                                       \
    {                                                                                                        \
        return name(policies, array, Scalar{value});                                                         \
    }                                                                                                        \
                                                                                                             \
                                                                                                             \
    /* f(policies, scalar, array) */                                                                         \
    template<typename Policies, Rank rank>                                                                   \
    auto name(                                                                                               \
        Policies const& policies,                                                                            \
        Scalar<policy::InputElementT<Policies, 0>> const& scalar,                                            \
        PartitionedArray<policy::InputElementT<Policies, 1>, rank> const& array)                             \
        -> PartitionedArray<policy::OutputElementT<Policies, 0>, rank>                                       \
    {                                                                                                        \
        return binary_local_operation(                                                                       \
            policies, scalar.future(), array, Functor<policy::InputElementT<Policies, 0>>{});                \
    }                                                                                                        \
                                                                                                             \
                                                                                                             \
    /* f(policies, value, array) */                                                                          \
    template<typename Policies, Rank rank>                                                                   \
    auto name(                                                                                               \
        Policies const& policies,                                                                            \
        policy::InputElementT<Policies, 0> const value,                                                      \
        PartitionedArray<policy::InputElementT<Policies, 1>, rank> const& array)                             \
        -> PartitionedArray<policy::OutputElementT<Policies, 0>, rank>                                       \
    {                                                                                                        \
        return name(policies, Scalar{value}, array);                                                         \
    }                                                                                                        \
                                                                                                             \
                                                                                                             \
    /* f(policies, scalar, scalar) */                                                                        \
    template<typename Policies>                                                                              \
    auto name(                                                                                               \
        Policies const& policies,                                                                            \
        Scalar<policy::InputElementT<Policies, 0>> const& scalar1,                                           \
        Scalar<policy::InputElementT<Policies, 1>> const& scalar2)                                           \
        -> Scalar<policy::OutputElementT<Policies, 0>>                                                       \
    {                                                                                                        \
        return binary_local_operation(                                                                       \
            policies, scalar1.future(), scalar2.future(), Functor<policy::InputElementT<Policies, 0>>{});    \
    }                                                                                                        \
                                                                                                             \
                                                                                                             \
    /* f(policies, scalar, value) */                                                                         \
    template<typename Policies>                                                                              \
    auto name(                                                                                               \
        Policies const& policies,                                                                            \
        Scalar<policy::InputElementT<Policies, 0>> const& scalar,                                            \
        policy::InputElementT<Policies, 1> const value) -> Scalar<policy::OutputElementT<Policies, 0>>       \
    {                                                                                                        \
        return name(policies, scalar, Scalar{value});                                                        \
    }                                                                                                        \
                                                                                                             \
                                                                                                             \
    /* f(policies, value, scalar) */                                                                         \
    template<typename Policies>                                                                              \
    auto name(                                                                                               \
        Policies const& policies,                                                                            \
        policy::InputElementT<Policies, 0> const value,                                                      \
        Scalar<policy::InputElementT<Policies, 1>> const& scalar)                                            \
        -> Scalar<policy::OutputElementT<Policies, 0>>                                                       \
    {                                                                                                        \
        return name(policies, Scalar{value}, scalar);                                                        \
    }                                                                                                        \
                                                                                                             \
                                                                                                             \
    /* f(policies, value, value) */                                                                          \
    template<typename Policies>                                                                              \
    auto name(                                                                                               \
        Policies const& policies,                                                                            \
        policy::InputElementT<Policies, 0> const value1,                                                     \
        policy::InputElementT<Policies, 1> const value2) -> Scalar<policy::OutputElementT<Policies, 0>>      \
    {                                                                                                        \
        return name(policies, Scalar{value1}, Scalar{value2});                                               \
    }


// All overloads *without* a Policies template parameter
#define LUE_BINARY_LOCAL_OPERATION_OVERLOADS_WITHOUT_POLICIES_DIFFERENT_OUTPUT_ELEMENT(name, Policies)       \
                                                                                                             \
    /* f(array, array) */                                                                                    \
    template<typename OutputElement, typename InputElement, Rank rank>                                       \
    auto name(                                                                                               \
        PartitionedArray<InputElement, rank> const& array1,                                                  \
        PartitionedArray<InputElement, rank> const& array2) -> PartitionedArray<OutputElement, rank>         \
    {                                                                                                        \
        using Policies_ = Policies<OutputElement, InputElement>;                                             \
                                                                                                             \
        return name(Policies_{}, array1, array2);                                                            \
    }                                                                                                        \
                                                                                                             \
                                                                                                             \
    /* f(array, scalar) */                                                                                   \
    template<typename OutputElement, typename InputElement, Rank rank>                                       \
    auto name(PartitionedArray<InputElement, rank> const& array, Scalar<InputElement> const& scalar)         \
        -> PartitionedArray<OutputElement, rank>                                                             \
    {                                                                                                        \
        using Policies_ = Policies<OutputElement, InputElement>;                                             \
                                                                                                             \
        return name(Policies_{}, array, scalar);                                                             \
    }                                                                                                        \
                                                                                                             \
                                                                                                             \
    /* f(array, value) */                                                                                    \
    template<typename OutputElement, typename InputElement, Rank rank>                                       \
    auto name(PartitionedArray<InputElement, rank> const& array, InputElement const value)                   \
        -> PartitionedArray<OutputElement, rank>                                                             \
    {                                                                                                        \
        using Policies_ = Policies<OutputElement, InputElement>;                                             \
                                                                                                             \
        return name(Policies_{}, array, value);                                                              \
    }                                                                                                        \
                                                                                                             \
                                                                                                             \
    /* f(scalar, array) */                                                                                   \
    template<typename OutputElement, typename InputElement, Rank rank>                                       \
    auto name(Scalar<InputElement> const& scalar, PartitionedArray<InputElement, rank> const& array)         \
        -> PartitionedArray<OutputElement, rank>                                                             \
    {                                                                                                        \
        using Policies_ = Policies<OutputElement, InputElement>;                                             \
                                                                                                             \
        return name(Policies_{}, scalar, array);                                                             \
    }                                                                                                        \
                                                                                                             \
                                                                                                             \
    /* f(value, array) */                                                                                    \
    template<typename OutputElement, typename InputElement, Rank rank>                                       \
    auto name(InputElement const value, PartitionedArray<InputElement, rank> const& array)                   \
        -> PartitionedArray<OutputElement, rank>                                                             \
    {                                                                                                        \
        using Policies_ = Policies<OutputElement, InputElement>;                                             \
                                                                                                             \
        return name(Policies_{}, value, array);                                                              \
    }                                                                                                        \
                                                                                                             \
                                                                                                             \
    /* f(scalar, scalar) */                                                                                  \
    template<typename OutputElement, typename InputElement>                                                  \
    auto name(Scalar<InputElement> const& scalar1, Scalar<InputElement> const& scalar2)                      \
        -> Scalar<OutputElement>                                                                             \
    {                                                                                                        \
        using Policies_ = Policies<OutputElement, InputElement>;                                             \
                                                                                                             \
        return name(Policies_{}, scalar1, scalar2);                                                          \
    }                                                                                                        \
                                                                                                             \
                                                                                                             \
    /* f(scalar, value) */                                                                                   \
    template<typename OutputElement, typename InputElement>                                                  \
    auto name(Scalar<InputElement> const& scalar, InputElement const value) -> Scalar<OutputElement>         \
    {                                                                                                        \
        using Policies_ = Policies<OutputElement, InputElement>;                                             \
                                                                                                             \
        return name(Policies_{}, scalar, value);                                                             \
    }                                                                                                        \
                                                                                                             \
                                                                                                             \
    /* f(value, scalar) */                                                                                   \
    template<typename OutputElement, typename InputElement>                                                  \
    auto name(InputElement const value, Scalar<InputElement> const& scalar) -> Scalar<OutputElement>         \
    {                                                                                                        \
        using Policies_ = Policies<OutputElement, InputElement>;                                             \
                                                                                                             \
        return name(Policies_{}, value, scalar);                                                             \
    }                                                                                                        \
                                                                                                             \
                                                                                                             \
    /* f(value, value) */                                                                                    \
    template<typename OutputElement, typename InputElement>                                                  \
        requires std::is_arithmetic_v<OutputElement> &&                                                      \
                 std::is_arithmetic_v<InputElement>                                                          \
                 auto name(InputElement const value1, InputElement const value2) -> Scalar<OutputElement>    \
    {                                                                                                        \
        using Policies_ = Policies<OutputElement, InputElement>;                                             \
                                                                                                             \
        return lue::name(Policies_{}, value1, value2);                                                       \
    }


// All overloads *without* a Policies template parameter
#define LUE_BINARY_LOCAL_OPERATION_OVERLOADS_WITHOUT_POLICIES_SAME_OUTPUT_ELEMENT(name, Policies)            \
                                                                                                             \
    /* f(array, array) */                                                                                    \
    template<typename Element, Rank rank>                                                                    \
    auto name(PartitionedArray<Element, rank> const& array1, PartitionedArray<Element, rank> const& array2)  \
        -> PartitionedArray<Element, rank>                                                                   \
    {                                                                                                        \
        using Policies_ = Policies<Element>;                                                                 \
                                                                                                             \
        return name(Policies_{}, array1, array2);                                                            \
    }                                                                                                        \
                                                                                                             \
                                                                                                             \
    /* f(array, scalar) */                                                                                   \
    template<typename Element, Rank rank>                                                                    \
    auto name(PartitionedArray<Element, rank> const& array, Scalar<Element> const& scalar)                   \
        -> PartitionedArray<Element, rank>                                                                   \
    {                                                                                                        \
        using Policies_ = Policies<Element>;                                                                 \
                                                                                                             \
        return name(Policies_{}, array, scalar);                                                             \
    }                                                                                                        \
                                                                                                             \
                                                                                                             \
    /* f(array, value) */                                                                                    \
    template<typename Element, Rank rank>                                                                    \
    auto name(PartitionedArray<Element, rank> const& array, Element const value)                             \
        -> PartitionedArray<Element, rank>                                                                   \
    {                                                                                                        \
        using Policies_ = Policies<Element>;                                                                 \
                                                                                                             \
        return name(Policies_{}, array, value);                                                              \
    }                                                                                                        \
                                                                                                             \
                                                                                                             \
    /* f(scalar, array) */                                                                                   \
    template<typename Element, Rank rank>                                                                    \
    auto name(Scalar<Element> const& scalar, PartitionedArray<Element, rank> const& array)                   \
        -> PartitionedArray<Element, rank>                                                                   \
    {                                                                                                        \
        using Policies_ = Policies<Element>;                                                                 \
                                                                                                             \
        return name(Policies_{}, scalar, array);                                                             \
    }                                                                                                        \
                                                                                                             \
                                                                                                             \
    /* f(value, array) */                                                                                    \
    template<typename Element, Rank rank>                                                                    \
    auto name(Element const value, PartitionedArray<Element, rank> const& array)                             \
        -> PartitionedArray<Element, rank>                                                                   \
    {                                                                                                        \
        using Policies_ = Policies<Element>;                                                                 \
                                                                                                             \
        return name(Policies_{}, value, array);                                                              \
    }                                                                                                        \
                                                                                                             \
                                                                                                             \
    /* f(scalar, scalar) */                                                                                  \
    template<typename Element>                                                                               \
    auto name(Scalar<Element> const& scalar1, Scalar<Element> const& scalar2) -> Scalar<Element>             \
    {                                                                                                        \
        using Policies_ = Policies<Element>;                                                                 \
                                                                                                             \
        return name(Policies_{}, scalar1, scalar2);                                                          \
    }                                                                                                        \
                                                                                                             \
                                                                                                             \
    /* f(scalar, value) */                                                                                   \
    template<typename Element>                                                                               \
    auto name(Scalar<Element> const& scalar, Element const value) -> Scalar<Element>                         \
    {                                                                                                        \
        using Policies_ = Policies<Element>;                                                                 \
                                                                                                             \
        return name(Policies_{}, scalar, value);                                                             \
    }                                                                                                        \
                                                                                                             \
                                                                                                             \
    /* f(value, scalar) */                                                                                   \
    template<typename Element>                                                                               \
    auto name(Element const value, Scalar<Element> const& scalar) -> Scalar<Element>                         \
    {                                                                                                        \
        using Policies_ = Policies<Element>;                                                                 \
                                                                                                             \
        return name(Policies_{}, value, scalar);                                                             \
    }                                                                                                        \
                                                                                                             \
                                                                                                             \
    /* f(value, value) */                                                                                    \
    template<typename Element>                                                                               \
        requires std::is_arithmetic_v<Element>                                                               \
    auto name(Element const value1, Element const value2) -> Scalar<Element>                                 \
    {                                                                                                        \
        using Policies_ = Policies<Element>;                                                                 \
                                                                                                             \
        return lue::name(Policies_{}, value1, value2);                                                       \
    }
