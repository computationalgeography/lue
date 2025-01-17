add_test_conditionally(test)

add_library(lue_qa
    source/scalability/array_experiment.cpp
    source/scalability/experiment.cpp
    source/scalability/io.cpp
    source/scalability/run.cpp
    source/scalability/stopwatch.cpp
    source/scalability/time_interval.cpp
)

add_library(lue::qa ALIAS lue_qa)

target_include_directories(lue_qa
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(lue_qa
    PRIVATE
        nlohmann_json::nlohmann_json
)
