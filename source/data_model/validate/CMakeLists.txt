add_library(lue_validate_lib STATIC
    source/validate.cpp
)

add_library(lue::validate ALIAS lue_validate_lib)

target_include_directories(lue_validate_lib
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(lue_validate_lib
    PUBLIC
        lue::utility
)

add_executable(lue_validate
    source/main.cpp
)

target_link_libraries(lue_validate
    PRIVATE
        lue::validate
)

lue_install_executables(
    TARGETS lue_validate
)
