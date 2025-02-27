add_test_conditionally(test)

add_library(lue_utility
    source/application.cpp
    source/command.cpp
    source/environment.cpp
    source/metadata.cpp
    source/print_message.cpp
    source/progress_indicator.cpp
    source/stopwatch.cpp
)

add_library(lue::utility ALIAS lue_utility)

target_include_directories(lue_utility
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(lue_utility
    PUBLIC
        lue::data_model
        cxxopts::cxxopts
        nlohmann_json::nlohmann_json
    PRIVATE
        lue::shared
)
