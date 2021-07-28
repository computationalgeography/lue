#include "lue/framework/algorithm/default_policies/cast.hpp"
#include "lue/framework/algorithm/definition/cast.hpp"
#include "lue/framework/algorithm/value_policies/cast.hpp"


#define LUE_INSTANTIATE(OutputElement, InputElement, rank)                  \
                                                                            \
    LUE_INSTANTIATE_CAST(                                                   \
        (policy::cast::DefaultPolicies<OutputElement, InputElement>),       \
        OutputElement,                                                      \
        InputElement,                                                       \
        rank);                                                              \
                                                                            \
    LUE_INSTANTIATE_CAST(                                                   \
        (policy::cast::DefaultValuePolicies<OutputElement, InputElement>),  \
        OutputElement,                                                      \
        InputElement,                                                       \
        rank);


namespace lue {

    // LUE_INSTANTIATE(std::uint32_t, bool, 2);
    // LUE_INSTANTIATE(std::int32_t, bool, 2);

    // LUE_INSTANTIATE(std::uint64_t, bool, 2);
    // LUE_INSTANTIATE(std::uint64_t, std::uint32_t, 2);
    // LUE_INSTANTIATE(std::int64_t, bool, 2);
    // LUE_INSTANTIATE(std::int64_t, std::int32_t, 2);

    // LUE_INSTANTIATE(float, bool, 2);
    // LUE_INSTANTIATE(float, std::int32_t, 2);
    // LUE_INSTANTIATE(float, std::uint32_t, 2);
    LUE_INSTANTIATE(float, std::uint64_t, 2);
    LUE_INSTANTIATE(float, std::int64_t, 2);

    // LUE_INSTANTIATE(double, bool, 2);
    // LUE_INSTANTIATE(double, std::int32_t, 2);
    // LUE_INSTANTIATE(double, std::uint32_t, 2);
    LUE_INSTANTIATE(double, std::uint64_t, 2);
    LUE_INSTANTIATE(double, std::int64_t, 2);

}  // namespace lue
