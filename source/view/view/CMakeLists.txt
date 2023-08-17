list(APPEND doxygen_input ${CMAKE_CURRENT_SOURCE_DIR})
set(doxygen_input ${doxygen_input} PARENT_SCOPE)

add_executable(lue_view
    src/application.cpp
    src/cache.cpp
    src/configuration.cpp
    src/dataset.cpp
    src/main.cpp
    src/show_dataset.cpp
    src/show_main_menu_bar.cpp

    $<IF:$<BOOL:${LUE_VIEW_USE_VULKAN}>,
        src/vulkan_application.cpp,
        src/opengl_application.cpp
    >
)

target_compile_options(lue_view
    PRIVATE
        # warning: format not a string literal and no format arguments
        $<$<CXX_COMPILER_ID:GNU>:-Wno-format-security>
)

target_include_directories(lue_view
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(lue_view
    PRIVATE
        lue::imgui
        lue::utility
        lue::shared
        glm::glm
)

if(LUE_VIEW_USE_VULKAN)
    set(shader_filenames
        shader.frag
        shader.vert
    )

    # TODO
    # To compile GLSL code to SPIR-V from within a program, see https://github.com/google/shaderc
    foreach(shader_filename ${shader_filenames})
        # Name of GLSL input file
        set(input_shader_pathname "${CMAKE_CURRENT_SOURCE_DIR}/shader/${shader_filename}")

        # Name of SPIR-V bytecode output file
        set(output_shader_pathname "shader/${shader_filename}.spv")

        add_custom_command(
            OUTPUT ${output_shader_pathname}
            COMMAND ${Vulkan_GLSLC_EXECUTABLE} ${input_shader_pathname} -o ${output_shader_pathname}
            DEPENDS ${input_shader_pathname}
            VERBATIM
            COMMENT "Create output shader ${output_shader_pathname}"
        )
        list(APPEND output_shaders_pathnames ${output_shader_pathname})
    endforeach()

    add_custom_target(shaders
        DEPENDS ${output_shaders_pathnames}
    )

    add_dependencies(lue_view shaders)
endif()

lue_install_executables(TARGETS lue_view)
