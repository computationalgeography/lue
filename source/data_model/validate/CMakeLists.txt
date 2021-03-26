add_library(lue_validate_lib STATIC
    src/validate.cpp
)

add_library(lue::validate ALIAS lue_validate_lib)

target_include_directories(lue_validate_lib
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(lue_validate_lib
    PUBLIC
        lue::utility
)

add_executable(lue_validate
    src/main.cpp
)

target_link_libraries(lue_validate
    PRIVATE
        lue::validate
)

install(
    TARGETS lue_validate
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
