#include "lue/framework/api/c.h"
#include "lue/document.h"
#include <stdio.h>
#include <stdlib.h>


int run_example(int const argc, char** argv)
{
    if (argc != 4)
    {
        printf("Wrong usage");
        return EXIT_FAILURE;
    }

    char const* argument_array_pathname = argv[1];
    char const* argument_kernel_pathname = argv[2];
    char const* result_array_pathname = argv[3];

    lue_initialize_example();

    // [example
    LUE_Field* array = lue_from_gdal(argument_array_pathname);
    LUE_Kernel* kernel = lue_read_kernel(argument_kernel_pathname);
    LUE_Field* result = lue_convolve(array, kernel);

    lue_to_gdal(result, result_array_pathname, NULL);

    lue_destruct(array);
    lue_destruct(kernel);
    lue_destruct(result);
    // example]

    return lue_finalize_runtime();
}


int main(int argc, char** argv)
{
    return lue_initialize_runtime(&run_example, argc, argv);
}
