#include "lue/framework/api/c.h"
#include "lue/document.h"
#include <stdio.h>
#include <stdlib.h>


int run_example(int const argc, char** argv)
{
    if (argc != 3)
    {
        printf("Wrong usage");
        return EXIT_FAILURE;
    }

    char const* elevation_array_pathname = argv[1];
    char const* flow_direction_array_pathname = argv[2];

    lue_initialize_example();

    // [example
    LUE_Field* elevation = lue_from_gdal(elevation_array_pathname);
    LUE_Field* flow_direction = lue_d8_flow_direction(elevation);

    lue_to_gdal(flow_direction, flow_direction_array_pathname, NULL);

    lue_destruct(elevation);
    lue_destruct(flow_direction);
    // example]

    return lue_finalize_runtime();
}


int main(int argc, char** argv)
{
    return lue_initialize_runtime(&run_example, argc, argv);
}
