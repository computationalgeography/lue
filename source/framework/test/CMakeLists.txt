add_library(lue_framework_test STATIC
    src/hpx_fixture.cpp
)

add_library(lue::framework_test ALIAS lue_framework_test)

# target_include_directories(lue_framework_test SYSTEM
#     PUBLIC
#         ${CONAN_INCLUDE_DIRS_GSL_MICROSOFT}  # span
# )

target_include_directories(lue_framework_test
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# target_link_libraries(lue_framework_test
#     PUBLIC
#         lue::framework
# )

target_link_libraries(lue_framework_test
    INTERFACE
        lue::framework_partitioned_array
)
