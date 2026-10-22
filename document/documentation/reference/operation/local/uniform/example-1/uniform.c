#include "lue/framework/api/c.h"
#include "lue/document.h"
#include <stdio.h>
#include <stdlib.h>


int run_example(int const argc, char** argv)
{
    if (argc != 2)
    {
        printf("Wrong usage");
        return EXIT_FAILURE;
    }

    char const* result_array_pathname = argv[1];

    lue_initialize_example();

    // [example
    typedef int32_t Element;
    LUE_Rank const rank = 2;
    LUE_Count const array_shape[] = {6, 4};

    LUE_Field* min_value = NULL;
    {
        Element const value = 1;
        LUE_Literal* literal = lue_create_literal(value);

        min_value = lue_as_field(literal);
    }

    LUE_Field* max_value = NULL;
    {
        Element const value = 10;
        LUE_Literal* literal = lue_create_literal(value);

        max_value = lue_as_field(literal);
    }

    LUE_Field* result = lue_uniform(rank, array_shape, min_value, max_value);

    lue_to_gdal(result, result_array_pathname, NULL);

    lue_destruct(min_value);
    lue_destruct(max_value);
    // example]

    // TODO: Results in crash. lue_to_gdal is still busy with the instance.
    // lue_destruct(result);

    return lue_finalize_runtime();
}


int main(int argc, char** argv)
{
    return lue_initialize_runtime(&run_example, argc, argv);
}
