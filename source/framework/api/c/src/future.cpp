#include "lue/framework/api/c/future.h"
#include "lue/framework/api/cxx/future.hpp"
#include "future.hpp"


void destruct_future(Future* future)
{
    if (future != nullptr)
    {
        if (future->instance != nullptr)
        {
            delete as_cxx_future_ptr(future);
        }

        delete future;
    }
}
