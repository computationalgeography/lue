#pragma once
#include "lue/framework/api/c/export.h"


#ifdef __cplusplus
extern "C" {
#endif


struct Field
{

        void* instance;
};


typedef struct Field Field;


LUE_FCAPI_EXPORT void destruct(Field* field);


#ifdef __cplusplus
}
#endif
