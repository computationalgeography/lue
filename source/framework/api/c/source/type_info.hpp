#pragma once
#include "lue/framework/api/c/type_info.h"
#include "lue/framework/api/cxx/type_info.hpp"


#define LUE_SWITCH_ON_ELEMENT_TYPE()                                                                         \
    switch (element_type)                                                                                    \
    {                                                                                                        \
        LUE_HANDLE_CASE(Int8)                                                                                \
        LUE_HANDLE_CASE(Int16)                                                                               \
        LUE_HANDLE_CASE(Int32)                                                                               \
        LUE_HANDLE_CASE(Int64)                                                                               \
        LUE_HANDLE_CASE(UInt8)                                                                               \
        LUE_HANDLE_CASE(UInt16)                                                                              \
        LUE_HANDLE_CASE(UInt32)                                                                              \
        LUE_HANDLE_CASE(UInt64)                                                                              \
        LUE_HANDLE_CASE(Float32)                                                                             \
        LUE_HANDLE_CASE(Float64)                                                                             \
    }


#define LUE_HANDLE_CASE(enum_)                                                                               \
    case (lue::ElementType::enum_):                                                                          \
    {                                                                                                        \
        result = LUE_ElementType_##enum_;                                                                    \
        break;                                                                                               \
    }


inline auto lue_from_cxx_enum(lue::ElementType const element_type) -> LUE_ElementType
{
    LUE_ElementType result{};
    LUE_SWITCH_ON_ELEMENT_TYPE()
    return result;
}


#undef LUE_HANDLE_CASE


#define LUE_HANDLE_CASE(enum_)                                                                               \
    case (LUE_ElementType_##enum_):                                                                          \
    {                                                                                                        \
        result = lue::ElementType::enum_;                                                                    \
        break;                                                                                               \
    }


inline auto lue_to_cxx_enum(LUE_ElementType const element_type) -> lue::ElementType
{
    lue::ElementType result{};
    LUE_SWITCH_ON_ELEMENT_TYPE()
    return result;
}


#undef LUE_HANDLE_CASE
#undef LUE_SWITCH_ON_ELEMENT_TYPE
