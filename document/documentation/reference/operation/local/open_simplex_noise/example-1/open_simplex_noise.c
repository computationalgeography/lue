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
    LUE_Rank const rank = 2;
    LUE_Count const array_shape[] = {600, 400};

    LUE_Field* condition_field = NULL;
    {
        LUE_BooleanElement const value = 1;
        LUE_Literal* literal = lue_create_literal(value);
        LUE_Scalar* scalar = lue_create_scalar(literal);
        LUE_Array* condition_array = lue_create_array(rank, array_shape, scalar);

        condition_field = lue_as_field(condition_array);

        lue_destruct(scalar);
        lue_destruct(literal);
    }

    LUE_Field* x_coordinates = NULL;
    {
        LUE_Field* const col_indices = lue_cell_index(condition_field, 1);

        x_coordinates = lue_cast(col_indices, LUE_ElementType_Float32);

        lue_destruct(col_indices);
    }

    LUE_Field* y_coordinates = NULL;
    {
        LUE_Field* const row_indices = lue_cell_index(condition_field, 0);

        y_coordinates = lue_cast(row_indices, LUE_ElementType_Float32);

        lue_destruct(row_indices);
    }

    int const seed = 5;
    LUE_Field* result = lue_open_simplex_noise(x_coordinates, y_coordinates, seed);

    lue_destruct(x_coordinates);
    lue_destruct(y_coordinates);

    lue_to_gdal(result, result_array_pathname, NULL);

    lue_destruct(result);
    // example]

    return lue_finalize_runtime();
}


int main(int argc, char** argv)
{
    return lue_initialize_runtime(&run_example, argc, argv);
}
