#pragma once
#include "lue/framework/api/c/export.h"


#ifdef __cplusplus
extern "C" {
#endif


struct Field
{

        /*!
            @brief      Opaque pointer to the C++ Field instance
        */
        void* instance;
};


typedef struct Field Field;


LUE_FCAPI_EXPORT void destruct_field(Field* field);


#ifdef __cplusplus
}
#endif
