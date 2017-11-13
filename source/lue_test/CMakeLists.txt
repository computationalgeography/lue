# add_test_conditionally(test)

add_library(lue_test STATIC
    src/dataset_fixture
)

add_library(lue::test ALIAS lue_test)

target_include_directories(lue_test
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(lue_test
    PUBLIC
        lue::lue
)

install(
    TARGETS
        lue_test
    EXPORT
        lue_targets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

install(
    DIRECTORY include
    DESTINATION .
)
