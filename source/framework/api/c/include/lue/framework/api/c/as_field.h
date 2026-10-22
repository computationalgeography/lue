#pragma once
#include "lue/framework/api/c/array.h"
#include "lue/framework/api/c/export.h"
#include "lue/framework/api/c/field.h"
#include "lue/framework/api/c/literal.h"
#include "lue/framework/api/c/scalar.h"


#ifdef __cplusplus
extern "C" {
#endif


LUE_FCAPI_EXPORT LUE_Field* lue_array_as_field(LUE_Array* array);

LUE_FCAPI_EXPORT LUE_Field* lue_literal_as_field(LUE_Literal* literal);

LUE_FCAPI_EXPORT LUE_Field* lue_scalar_as_field(LUE_Scalar* scalar);


#define lue_as_field(X)                                                                                      \
    _Generic(                                                                                                \
        (X),                                                                                                 \
        LUE_Array*: lue_array_as_field,                                                                      \
        LUE_Literal*: lue_literal_as_field,                                                                  \
        LUE_Scalar*: lue_scalar_as_field)(X)


#ifdef __cplusplus
}
#endif
