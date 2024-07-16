#include "lue/framework/api/c/io.h"


int main()
{
    // TODO Start HPX runtime

    {
        Field* result = from_gdal("x.tif");

        // TODO Test result

        destruct(result);
    }

    // TODO Stop HPX runtime
}
