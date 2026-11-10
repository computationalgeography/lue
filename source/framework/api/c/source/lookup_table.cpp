#include "lue/framework/api/c/lookup_table.h"
#include "lookup_table.hpp"


void lue_destruct_lookup_table(LUE_LookupTable* lookup_table)
{
    if (lookup_table != nullptr)
    {
        if (lookup_table->instance != nullptr)
        {
            delete lue_as_cxx_lookup_table_ptr(lookup_table);
        }

        delete lookup_table;
    }
}
