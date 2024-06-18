#include "lue/framework/api/c/field.h"
#include "lue/framework/api/cxx/field.hpp"


void destruct(Field* field)
{
    if (field != nullptr)
    {
        if (field->instance != nullptr)
        {
            auto* instance = static_cast<lue::api::Field*>(field->instance);
            delete instance;
        }
    }

    delete field;
}
