add_library(lue_utility SHARED)
add_library(lue::utility ALIAS lue_utility)

set_target_properties(lue_utility
    PROPERTIES
        EXPORT_NAME utility
        VERSION ${LUE_VERSION}
        SOVERSION ${LUE_VERSION_MAJOR}
        DLL_NAME_WITH_SOVERSION TRUE
)

generate_export_header(lue_utility
    EXPORT_FILE_NAME include/lue/utility/export.hpp
)

target_sources(lue_utility
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS
            include
            ${CMAKE_CURRENT_BINARY_DIR}/include
        FILES
            include/lue
            ${CMAKE_CURRENT_BINARY_DIR}/include/lue/utility/export.hpp
    PRIVATE
        source/application.cpp
        source/command.cpp
        source/environment.cpp
        source/metadata.cpp
        source/print_message.cpp
        source/progress_indicator.cpp
        source/stopwatch.cpp
)

target_link_libraries(lue_utility
    PUBLIC
        lue::data_model
        cxxopts::cxxopts
        nlohmann_json::nlohmann_json
    PRIVATE
        lue::shared
)

lue_install_runtime_libraries(
    TARGETS
        lue_utility
)

# TODO: cxxopts is not exported ...
# lue_install_development_libraries(
#     TARGETS
#         lue_utility
# )

if(LUE_BUILD_TESTS)
    lue_configure_static_library_for_tests(
        TARGET_NAME_SHARED_LIB lue_utility
        TARGET_NAME_STATIC_LIB lue_utility_static
        EXPORT_MACRO_BASENAME UTILITY
    )

    add_subdirectory(test)
endif()
