set(names
    scalability/array_experiment
    scalability/experiment
    scalability/run
    scalability/stopwatch
)

add_unit_tests(
    SCOPE lue_qa
    NAMES ${names}
    LIBRARIES
        lue::qa
        lue::shared
    TARGETS
        test_names
)
