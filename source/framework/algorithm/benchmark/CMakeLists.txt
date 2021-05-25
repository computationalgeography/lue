lue_add_benchmark(CATEGORY algorithm NAME accu_threshold
    LINK_LIBRARIES
        lue::data_model_hl
        lue::framework_flow_direction_operation
)

lue_add_benchmark(CATEGORY algorithm NAME copy)
lue_add_benchmark(CATEGORY algorithm NAME empty)
lue_add_benchmark(CATEGORY algorithm NAME focal_mean)
lue_add_benchmark(CATEGORY algorithm NAME iterate_per_element)
lue_add_benchmark(CATEGORY algorithm NAME multiply)
lue_add_benchmark(CATEGORY algorithm NAME sqrt)
lue_add_benchmark(CATEGORY algorithm NAME zonal_sum)
