#pragma once
#include "lue/framework/api/c/export.h"
#include "lue/framework/api/c/field.h"


#ifdef __cplusplus
extern "C" {
#endif


LUE_FCAPI_EXPORT Field* add(Field const* field1, Field const* field2);

LUE_FCAPI_EXPORT Field* close_to(Field const* field1, Field const* field2);

LUE_FCAPI_EXPORT Field* equal_to(Field const* field1, Field const* field2);

LUE_FCAPI_EXPORT Field* greater_than(Field const* field1, Field const* field2);

LUE_FCAPI_EXPORT Field* greater_than_equal_to(Field const* field1, Field const* field2);

LUE_FCAPI_EXPORT Field* less_than(Field const* field1, Field const* field2);

LUE_FCAPI_EXPORT Field* less_than_equal_to(Field const* field1, Field const* field2);

LUE_FCAPI_EXPORT Field* logical_and(Field const* field1, Field const* field2);

LUE_FCAPI_EXPORT Field* logical_exclusive_or(Field const* field1, Field const* field2);

LUE_FCAPI_EXPORT Field* logical_inclusive_or(Field const* field1, Field const* field2);

LUE_FCAPI_EXPORT Field* logical_not(Field const* field);

LUE_FCAPI_EXPORT Field* not_equal_to(Field const* field1, Field const* field2);


#ifdef __cplusplus
}
#endif
