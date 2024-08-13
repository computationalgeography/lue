#pragma once
#include "lue/framework/api/c/export.h"
#include "lue/framework/api/c/field.h"


#ifdef __cplusplus
extern "C" {
#endif


LUE_FCAPI_EXPORT Field* add(Field const* field1, Field const* field2);


#ifdef __cplusplus
}
#endif
