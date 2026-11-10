#pragma once
#include "lue/framework/api/c/export.h"


#ifdef __cplusplus
extern "C" {
#endif


struct LUE_LookupTable
{

        /*!
            @brief      Opaque pointer to the C++ LookupTable instance
        */
        void* instance;
};


typedef struct LUE_LookupTable LUE_LookupTable;


LUE_FCAPI_EXPORT void lue_destruct_lookup_table(LUE_LookupTable* lookup_table);


#ifdef __cplusplus
}
#endif
