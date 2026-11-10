#include "hpx_unit_test.h"
#include "lue/framework/api/c/as_field.h"
#include "lue/framework/api/c/destruct.h"
#include "lue/framework/api/c/miscellaneous_operation.h"
#include "lue/framework/api/c/type_info.h"


// TODO: All this assumes the used element types are supported. We need a C header with information about the
//       supported types. Picking the wrong type here results in link errors.


static void array_like_test(void)
{
    typedef float InputElement;
    LUE_Array* input_array = NULL;

    {
        LUE_Rank const rank = 2;
        LUE_Count const array_shape[] = {60, 40};
        InputElement const value = 5.5F;
        LUE_Literal* literal = lue_create_literal(value);
        LUE_Scalar* scalar = lue_create_scalar(literal);

        input_array = lue_create_array(rank, array_shape, scalar);

        lue_destruct(scalar);
        lue_destruct(literal);
    }

    typedef int32_t OutputElement;
    LUE_Scalar* fill_value = NULL;

    {
        OutputElement const value = 3;
        LUE_Literal* literal = lue_create_literal(value);

        fill_value = lue_create_scalar(literal);

        lue_destruct(literal);
    }

    LUE_Array* output_array = lue_array_like(input_array, fill_value);

    CU_ASSERT_NOT_EQUAL(output_array, NULL);
    CU_ASSERT_EQUAL(lue_element_type(output_array), LUE_ElementType_Int32);

    lue_destruct(input_array);
    lue_destruct(fill_value);
    lue_destruct(output_array);
}


static void cast_test(void)
{
    typedef float InputElement;
    LUE_Field* input_field = NULL;

    {
        LUE_Rank const rank = 2;
        LUE_Count const array_shape[] = {60, 40};
        InputElement const value = 5.5F;
        LUE_Literal* literal = lue_create_literal(value);
        LUE_Scalar* scalar = lue_create_scalar(literal);
        LUE_Array* array = lue_create_array(rank, array_shape, scalar);

        input_field = lue_as_field(array);

        lue_destruct(scalar);
        lue_destruct(literal);
    }

    LUE_Field* output_field = lue_cast(input_field, LUE_ElementType_Int32);

    CU_ASSERT_NOT_EQUAL(output_field, NULL);
    CU_ASSERT_EQUAL(lue_data_model(output_field), LUE_DataModel_Array);
    CU_ASSERT_EQUAL(lue_element_type(output_field), LUE_ElementType_Int32);

    lue_destruct(input_field);
    lue_destruct(output_field);
}


static void cell_index_test(void)
{
    typedef LUE_BooleanElement InputElement;
    LUE_Field* input_field = NULL;

    {
        LUE_Rank const rank = 2;
        LUE_Count const array_shape[] = {60, 40};
        InputElement const value = 1;
        LUE_Literal* literal = lue_create_literal(value);
        LUE_Scalar* scalar = lue_create_scalar(literal);
        LUE_Array* array = lue_create_array(rank, array_shape, scalar);

        input_field = lue_as_field(array);

        lue_destruct(scalar);
        lue_destruct(literal);
    }

    LUE_Field* output_field = lue_cell_index(input_field, 0);

    CU_ASSERT_NOT_EQUAL(output_field, NULL);
    CU_ASSERT_EQUAL(lue_data_model(output_field), LUE_DataModel_Array);
    CU_ASSERT_EQUAL(lue_element_type(output_field), LUE_ElementType_Index);

    lue_destruct(input_field);
    lue_destruct(output_field);
}


static void create_array_test(void)
{
    typedef float Element;

    LUE_Rank const rank = 2;
    LUE_Count const array_shape[] = {60, 40};

    Element const value = 5.5F;
    LUE_Literal* literal = lue_create_literal(value);
    LUE_Scalar* scalar = lue_create_scalar(literal);
    LUE_Array* array = lue_create_array(rank, array_shape, scalar);

    CU_ASSERT_NOT_EQUAL(array, NULL);
    CU_ASSERT_EQUAL(lue_element_type(array), LUE_ElementType_Float32);

    lue_destruct(array);
    lue_destruct(scalar);
    lue_destruct(literal);
}


static void create_literal_test(void)
{
    typedef uint8_t Element;

    Element const value = 5;
    LUE_Literal* literal = lue_create_literal(value);

    CU_ASSERT_NOT_EQUAL(literal, NULL);
    CU_ASSERT_EQUAL(lue_element_type(literal), LUE_ElementType_UInt8);

    lue_destruct(literal);
}


static void create_scalar_test(void)
{
    typedef int32_t Element;

    Element const value = -5;
    LUE_Literal* literal = lue_create_literal(value);
    LUE_Scalar* scalar = lue_create_scalar(literal);

    CU_ASSERT_NOT_EQUAL(scalar, NULL);
    CU_ASSERT_EQUAL(lue_element_type(scalar), LUE_ElementType_Int32);

    lue_destruct(scalar);
    lue_destruct(literal);
}


// TODO: reclassify


HPX_UNIT_TEST_SUITE(
    "miscellaneous_operation",
    CUNIT_CI_TEST(array_like_test),
    CUNIT_CI_TEST(cast_test),
    CUNIT_CI_TEST(cell_index_test),
    CUNIT_CI_TEST(create_array_test),
    CUNIT_CI_TEST(create_literal_test),
    CUNIT_CI_TEST(create_scalar_test));
