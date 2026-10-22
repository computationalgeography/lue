#pragma once
#include "lue/framework/api/c/runtime.h"
#include <CUnit/CUnitCI.h>


CU_SUITE_TEARDOWN()
{
    int hpx_status = lue_finalize_runtime();

    return hpx_status == 0 ? CUE_SUCCESS : CUE_SCLEAN_FAILED;
}


#define HPX_UNIT_TEST_SUITE(suite_name, ...)                                                                 \
                                                                                                             \
    int test_suite(int argc, char** argv)                                                                    \
    {                                                                                                        \
        CU_CI_add_suite(                                                                                     \
            suite_name, __cu_suite_setup, __cu_suite_teardown, __cu_test_setup, __cu_test_teardown);         \
                                                                                                             \
        __VA_ARGS__;                                                                                         \
                                                                                                             \
        return CU_CI_main(argc, argv);                                                                       \
    }                                                                                                        \
                                                                                                             \
                                                                                                             \
    int main(int argc, char** argv)                                                                          \
    {                                                                                                        \
        /* init function that runs the whole test suite. Once it finishes, finalize can be called */         \
        return lue_initialize_runtime(&test_suite, argc, argv);                                              \
    }
