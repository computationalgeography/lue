add_test_conditionally(test)

add_library(lue_utility
    src/application.cpp
    src/command.cpp
    src/environment.cpp
    src/metadata.cpp
    src/print_message.cpp
    src/progress_indicator.cpp
    src/stopwatch.cpp
)

add_library(lue::utility ALIAS lue_utility)

target_include_directories(lue_utility
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(lue_utility
    PUBLIC
        lue::data_model
        docopt::docopt
        nlohmann_json::nlohmann_json
)
