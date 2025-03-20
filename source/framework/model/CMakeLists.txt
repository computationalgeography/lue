add_library(lue_framework_model STATIC)
add_library(lue::framework_model ALIAS lue_framework_model)

target_sources(lue_framework_model
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS
            include
        FILES
            include/lue
    PRIVATE
        source/model.cpp
        source/progressor.cpp
)

target_link_libraries(lue_framework_model
    PUBLIC
        lue::framework_core
)

if(LUE_BUILD_TESTS)
    add_subdirectory(test)
endif()
