add_test_conditionally(test)

add_library(lue_framework_api_c SHARED
    source/field.cpp
    source/future.cpp
    source/io.cpp
    source/local_operation.cpp
)

lue_configure_shared_library(
    TARGET_BASENAME framework_api_c
    EXPORT_HEADER_PATHNAME include/lue/framework/api/c/export.hpp
    EXPORT_MACRO_BASENAME FCAPI
)

target_link_libraries(lue_framework_api_c
    PRIVATE
        lue::framework_api_cxx
)
