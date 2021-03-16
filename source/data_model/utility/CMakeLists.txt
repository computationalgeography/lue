add_test_conditionally(test)

add_library(lue_utility
    src/application
    src/command
    src/environment
    src/metadata
    src/print_message
    src/progress_indicator
    src/stopwatch
)

add_library(lue::utility ALIAS lue_utility)

target_include_directories(lue_utility
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    # PRIVATE
    #     fmt::fmt
)

target_link_libraries(lue_utility
    PUBLIC
        lue::data_model
        docopt::docopt
        nlohmann_json::nlohmann_json
    # PRIVATE
    #     fmt::fmt
)
