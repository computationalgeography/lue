add_library(lue_utility
    src/application
    src/command
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
)

target_link_libraries(lue_utility
    PUBLIC
        docopt
        nlohmann_json
    PRIVATE
        lue::hl
)
