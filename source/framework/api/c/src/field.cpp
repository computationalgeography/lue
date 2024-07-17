#include "lue/framework/api/c/field.h"
#include "lue/framework/api/cxx/field.hpp"
#include "field.hpp"


void destruct(Field* field)
{
    if (field != nullptr)
    {
        if (field->instance != nullptr)
        {
            delete as_cxx_field_ptr(field);
        }

        delete field;
    }
}
