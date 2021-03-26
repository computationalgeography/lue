add_hpx_executable(lue_describe_flow_direction
    ESSENTIAL
    SOURCES src/describe_flow_direction.cpp
    src/main.cpp
)


# add_executable(lue_describe_flow_direction
#     src/describe_flow_direction.cpp
#     src/main.cpp
# )

target_link_libraries(lue_describe_flow_direction
    PRIVATE
        lue::data_model_hl
        lue::framework_core
        lue::framework_io
        lue::utility
)
