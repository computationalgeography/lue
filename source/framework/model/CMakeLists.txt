add_library(lue_framework_model SHARED)
add_library(lue::framework_model ALIAS lue_framework_model)

set_target_properties(lue_framework_model
    PROPERTIES
        EXPORT_NAME framework_model
        VERSION ${LUE_VERSION}
        SOVERSION ${LUE_VERSION_MAJOR}
        DLL_NAME_WITH_SOVERSION TRUE
)

generate_export_header(lue_framework_model
    EXPORT_FILE_NAME include/lue/framework/model/export.hpp
)

target_sources(lue_framework_model
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS
            include
            ${CMAKE_CURRENT_BINARY_DIR}/include
        FILES
            include/lue
            ${CMAKE_CURRENT_BINARY_DIR}/include/lue/framework/model/export.hpp
    PRIVATE
        source/model.cpp
        source/progressor.cpp
)

target_link_libraries(lue_framework_model
    PUBLIC
        lue::framework_core
)

lue_install_runtime_libraries(
    TARGETS
        lue_framework_model
)

lue_install_development_libraries(
    TARGETS
        lue_framework_model
)

if(LUE_BUILD_TESTS)
    add_subdirectory(test)
endif()
