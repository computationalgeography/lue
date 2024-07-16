#pragma once
#include "lue/framework/api/c/export.h"
#include "lue/framework/api/c/field.h"


#ifdef __cplusplus
extern "C" {
#endif


LUE_FCAPI_EXPORT Field* from_gdal(char const* name);


#ifdef __cplusplus
}
#endif
