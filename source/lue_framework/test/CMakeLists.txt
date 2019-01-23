add_library(lue_framework_test STATIC
    src/hpx_fixture
)

add_library(lue::framework_test ALIAS lue_framework_test)

target_include_directories(lue_framework_test
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# target_link_libraries(lue_framework_test
#     PUBLIC
#         lue::framework
# )
