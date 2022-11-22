lue_add_benchmark(CATEGORY algorithm NAME accu_threshold
    LINK_LIBRARIES
        lue::data_model_hl
        lue::framework_flow_direction_operation
        lue::framework_io
)
lue_add_benchmark(CATEGORY algorithm NAME accu_threshold3
    LINK_LIBRARIES
        lue::framework_flow_direction_operation
        lue::framework_io
)
lue_add_benchmark(CATEGORY algorithm NAME accu3
    LINK_LIBRARIES
        lue::framework_flow_direction_operation
        lue::framework_io
)


lue_add_benchmark(CATEGORY algorithm NAME copy)
lue_add_benchmark(CATEGORY algorithm NAME empty)
lue_add_benchmark(CATEGORY algorithm NAME focal_mean LINK_LIBRARIES lue::framework_focal_operation)
lue_add_benchmark(CATEGORY algorithm NAME iterate_per_element)
lue_add_benchmark(CATEGORY algorithm NAME multiply LINK_LIBRARIES lue::framework_local_operation)
lue_add_benchmark(CATEGORY algorithm NAME sqrt LINK_LIBRARIES lue::framework_local_operation)
lue_add_benchmark(CATEGORY algorithm NAME zonal_sum LINK_LIBRARIES lue::framework_zonal_operation)
