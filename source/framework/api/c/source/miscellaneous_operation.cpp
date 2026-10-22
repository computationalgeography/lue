#include "lue/framework/api/c/miscellaneous_operation.h"
#include "array.hpp"
#include "field.hpp"
#include "literal.hpp"
#include "scalar.hpp"
#include "type_info.hpp"
#include "lue/framework/api/cxx/miscellaneous_operation.hpp"
#include "lue/configure.hpp"


auto lue_cast(LUE_Field* field, LUE_ElementType const element_type) -> LUE_Field*
{
    lue::api::Field result = lue::api::cast(lue_as_cxx_field(field), lue_to_cxx_enum(element_type));

    return new LUE_Field{.instance = new lue::api::Field{std::move(result)}};
}


auto lue_cell_index(LUE_Field* field, LUE_Index const dimension_index) -> LUE_Field*
{
    lue::api::Field result = lue::api::cell_index(lue_as_cxx_field(field), dimension_index);

    return new LUE_Field{.instance = new lue::api::Field{std::move(result)}};
}


auto lue_create_array(LUE_Rank const rank, LUE_Count const* array_shape_p, LUE_Scalar const* fill_value)
    -> LUE_Array*
{
    assert(rank == 2);

    lue::Shape<lue::Count, 2> array_shape{};
    std::copy(array_shape_p, array_shape_p + rank, array_shape.data());

    lue::api::Array result = lue::api::create_array(array_shape, lue_as_cxx_scalar(fill_value));

    return new LUE_Array{.instance = new lue::api::Array{std::move(result)}};
}


auto lue_create_box_kernel(LUE_Count const radius, LUE_Literal const* weight) -> LUE_Kernel*
{
    lue::api::Kernel result = lue::api::create_box_kernel(radius, lue_as_cxx_literal(weight));

    return new LUE_Kernel{.instance = new lue::api::Kernel{std::move(result)}};
}


#define CreateLiteral(type, type_name)                                                                       \
    auto lue_create_literal_##type_name(type const value) -> LUE_Literal*                                    \
    {                                                                                                        \
        lue::api::Literal result{value};                                                                     \
                                                                                                             \
        return new LUE_Literal{.instance = new lue::api::Literal{std::move(result)}};                        \
    }


#ifdef LUE_FRAMEWORK_WITH_UINT8_ELEMENT
CreateLiteral(uint8_t, uint8);
#endif

#ifdef LUE_FRAMEWORK_WITH_UINT16_ELEMENT
CreateLiteral(uint16_t, uint16);
#endif

#ifdef LUE_FRAMEWORK_WITH_UINT32_ELEMENT
CreateLiteral(uint32_t, uint32);
#endif

#ifdef LUE_FRAMEWORK_WITH_UINT64_ELEMENT
CreateLiteral(uint64_t, uint64);
#endif

#ifdef LUE_FRAMEWORK_WITH_INT8_ELEMENT
CreateLiteral(int8_t, int8);
#endif

#ifdef LUE_FRAMEWORK_WITH_INT16_ELEMENT
CreateLiteral(int16_t, int16);
#endif

#ifdef LUE_FRAMEWORK_WITH_INT32_ELEMENT
CreateLiteral(int32_t, int32);
#endif

#ifdef LUE_FRAMEWORK_WITH_INT64_ELEMENT
CreateLiteral(int64_t, int64);
#endif

#ifdef LUE_FRAMEWORK_WITH_FLOAT32_ELEMENT
CreateLiteral(float, float32);
#endif

#ifdef LUE_FRAMEWORK_WITH_FLOAT64_ELEMENT
CreateLiteral(double, float64);
#endif

#undef CreateLiteral


auto lue_array_like(LUE_Array* array, LUE_Scalar* fill_value) -> LUE_Array*
{
    lue::api::Array result = lue::api::array_like(lue_as_cxx_array(array), lue_as_cxx_scalar(fill_value));

    return new LUE_Array{.instance = new lue::api::Array{std::move(result)}};
}


auto lue_create_scalar(LUE_Literal* fill_value) -> LUE_Scalar*
{
    lue::api::Scalar result = lue::api::create_scalar(lue_as_cxx_literal(fill_value));

    return new LUE_Scalar{.instance = new lue::api::Scalar{std::move(result)}};
}
