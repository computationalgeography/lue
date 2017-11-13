add_library(lue_utility
    src/application
    src/command
    src/metadata
    src/print_message
    src/progress_indicator
    src/stopwatch
)

add_library(lue::utility ALIAS lue_utility)

target_include_directories(lue_utility SYSTEM
    PUBLIC
        ${DOCOPT_INCLUDE_DIRS}
        ${JSON_INCLUDE_DIR}
)

target_include_directories(lue_utility
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
        lue::time
)

target_link_libraries(lue_utility
    PRIVATE
        lue::hl
        ${DOCOPT_LIBRARIES}
)
