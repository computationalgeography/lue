add_library(lue_validate_lib STATIC)
add_library(lue::validate ALIAS lue_validate_lib)

target_sources(lue_validate_lib
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS
            include
        FILES
            include/lue
    PRIVATE
        source/validate.cpp
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
