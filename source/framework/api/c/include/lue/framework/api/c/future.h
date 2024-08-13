#pragma once
#include "lue/framework/api/c/export.h"


#ifdef __cplusplus
extern "C" {
#endif


struct Future
{

        /*!
            @brief      Opaque pointer to the C++ lue::api::Future instance
        */
        void* instance;
};


typedef struct Future Future;


LUE_FCAPI_EXPORT void destruct_future(Future* future);


#ifdef __cplusplus
}
#endif
