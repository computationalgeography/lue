#include "lue/framework/algorithm/default_policies/equal_to.hpp"
#include "lue/framework/algorithm/definition/equal_to.hpp"
#include "lue/framework/algorithm/value_policies/equal_to.hpp"


// NOTE
// The overload for partitions is used by array_partition_id test. If
// not used elsewhere, get rid of it. If we need to be able to call
// operations for partitions instead of array, then keep it.

#define LUE_INSTANTIATE(OutputElement, InputElement, rank)                      \
                                                                                \
    LUE_INSTANTIATE_BINARY_LOCAL_OPERATION_PARTITION(                           \
        (policy::equal_to::DefaultPolicies<OutputElement, InputElement>),       \
        OutputElement,                                                          \
        InputElement,                                                           \
        InputElement,                                                           \
        rank,                                                                   \
        (detail::EqualTo<InputElement, OutputElement>));                        \
                                                                                \
    LUE_INSTANTIATE_BINARY_LOCAL_OPERATION(                                     \
        (policy::equal_to::DefaultPolicies<OutputElement, InputElement>),       \
        OutputElement,                                                          \
        InputElement,                                                           \
        InputElement,                                                           \
        rank,                                                                   \
        (detail::EqualTo<InputElement, OutputElement>));                        \
                                                                                \
    LUE_INSTANTIATE_BINARY_LOCAL_OPERATION(                                     \
        (policy::equal_to::DefaultValuePolicies<OutputElement, InputElement>),  \
        OutputElement,                                                          \
        InputElement,                                                           \
        InputElement,                                                           \
        rank,                                                                   \
        (detail::EqualTo<InputElement, OutputElement>));


namespace lue {

    LUE_INSTANTIATE(std::uint8_t, std::uint8_t, 2);
    LUE_INSTANTIATE(std::uint8_t, std::uint32_t, 2);
    LUE_INSTANTIATE(std::uint8_t, std::uint64_t, 2);
    LUE_INSTANTIATE(std::uint8_t, std::int32_t, 2);
    LUE_INSTANTIATE(std::uint8_t, std::int64_t, 2);
    LUE_INSTANTIATE(std::uint8_t, float, 2);
    LUE_INSTANTIATE(std::uint8_t, double, 2);

}  // namespace lue
