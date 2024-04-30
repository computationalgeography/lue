#include "lue/framework/api/c/local_operation.h"


int main()
{
    // TODO Start HPX runtime

    // TODO
    Field* field1 = 0;  //  = uniform(...);
    Field* field2 = 0;  //  = uniform(...);

    {
        Field* result = add(field1, field2);

        // TODO Test result

        destruct(result);
    }

    destruct(field1);
    destruct(field2);

    // TODO Stop HPX runtime
}
