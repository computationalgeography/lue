add_library(lue_utility STATIC)
add_library(lue::utility ALIAS lue_utility)

target_sources(lue_utility
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS
            include
        FILES
            include/lue
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

if(LUE_BUILD_TESTS)
    add_subdirectory(test)
endif()
