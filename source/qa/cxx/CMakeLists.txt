add_test_conditionally(test)

add_library(lue_qa
    src/scalability/array_experiment.cpp
    src/scalability/experiment.cpp
    src/scalability/io.cpp
    src/scalability/run.cpp
    src/scalability/stopwatch.cpp
    src/scalability/time_interval.cpp
)

add_library(lue::qa ALIAS lue_qa)

target_include_directories(lue_qa
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(lue_qa
    PUBLIC
        # lue::framework_algorithm
        # lue::framework_model
        # lue::data_model_hl
        docopt
        nlohmann_json::nlohmann_json
)
