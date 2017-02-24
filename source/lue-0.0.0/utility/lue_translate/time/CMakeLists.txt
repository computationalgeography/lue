add_test_conditionally(test)

set(SOURCES
    month_duration
    month_time_point
    from_string
    to_string
)
add_library(lue_translate_time ${SOURCES})
