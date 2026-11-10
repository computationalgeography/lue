#pragma once
#include "lue/framework/api/c/array.h"
#include "lue/framework/api/c/define.h"
#include "lue/framework/api/c/export.h"
#include "lue/framework/api/c/field.h"
#include "lue/framework/api/c/kernel.h"
#include "lue/framework/api/c/literal.h"
#include "lue/framework/api/c/lookup_table.h"
#include "lue/framework/api/c/scalar.h"
#include "lue/framework/api/c/type_info.h"
#include <stddef.h>
#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif


LUE_FCAPI_EXPORT LUE_Array* lue_array_like(LUE_Array* array, LUE_Scalar* fill_value);

LUE_FCAPI_EXPORT LUE_Field* lue_cast(LUE_Field* field, LUE_ElementType element_type);

LUE_FCAPI_EXPORT LUE_Field* lue_cell_index(LUE_Field* field, LUE_Index dimension_index);

LUE_FCAPI_EXPORT LUE_Array* lue_create_array(
    LUE_Rank rank, LUE_Count const* array_shape, LUE_Scalar const* fill_value);

LUE_FCAPI_EXPORT LUE_Kernel* lue_create_box_kernel(LUE_Count radius, LUE_Literal const* weight);


LUE_FCAPI_EXPORT LUE_Literal* lue_create_literal_uint8(uint8_t value);
LUE_FCAPI_EXPORT LUE_Literal* lue_create_literal_uint16(uint16_t value);
LUE_FCAPI_EXPORT LUE_Literal* lue_create_literal_uint32(uint32_t value);
LUE_FCAPI_EXPORT LUE_Literal* lue_create_literal_uint64(uint64_t value);
LUE_FCAPI_EXPORT LUE_Literal* lue_create_literal_int8(int8_t value);
LUE_FCAPI_EXPORT LUE_Literal* lue_create_literal_int16(int16_t value);
LUE_FCAPI_EXPORT LUE_Literal* lue_create_literal_int32(int32_t value);
LUE_FCAPI_EXPORT LUE_Literal* lue_create_literal_int64(int64_t value);
LUE_FCAPI_EXPORT LUE_Literal* lue_create_literal_float32(float value);
LUE_FCAPI_EXPORT LUE_Literal* lue_create_literal_float64(double value);


#define lue_create_literal(X)                                                                                \
    _Generic(                                                                                                \
        (X),                                                                                                 \
        uint8_t: lue_create_literal_uint8,                                                                   \
        uint16_t: lue_create_literal_uint16,                                                                 \
        uint32_t: lue_create_literal_uint32,                                                                 \
        uint64_t: lue_create_literal_uint64,                                                                 \
        int8_t: lue_create_literal_int8,                                                                     \
        int16_t: lue_create_literal_int16,                                                                   \
        int32_t: lue_create_literal_int32,                                                                   \
        int64_t: lue_create_literal_int64,                                                                   \
        float: lue_create_literal_float32,                                                                   \
        double: lue_create_literal_float64)(X)

LUE_FCAPI_EXPORT LUE_Scalar* lue_create_scalar(LUE_Literal* value);

LUE_FCAPI_EXPORT LUE_Field* lue_reclassify(LUE_Field* field, LUE_LookupTable* lookup_table);

#ifdef __cplusplus
}
#endif
