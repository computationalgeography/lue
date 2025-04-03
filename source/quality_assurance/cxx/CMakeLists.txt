add_library(lue_qa STATIC)
add_library(lue::qa ALIAS lue_qa)

target_sources(lue_qa
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS
            include
        FILES
            include/lue
    PRIVATE
        source/scalability/array_experiment.cpp
        source/scalability/experiment.cpp
        source/scalability/io.cpp
        source/scalability/run.cpp
        source/scalability/stopwatch.cpp
        source/scalability/time_interval.cpp
)

target_link_libraries(lue_qa
    PUBLIC
        lue::compile_features
    PRIVATE
        nlohmann_json::nlohmann_json
)

if(LUE_BUILD_TESTS)
    add_subdirectory(test)
endif()
