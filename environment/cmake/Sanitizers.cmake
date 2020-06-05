option(${PROJECT_NAME}_ENABLE_ADDRESS_SANITIZER
    "Enable address sanitizer" OFF)
option(${PROJECT_NAME}_ENABLE_MEMORY_SANITIZER
    "Enable memory sanitizer" OFF)
option(${PROJECT_NAME}_ENABLE_UNDEFINED_BEHAVIOR_SANITIZER
    "Enable undefined behavior sanitizer" OFF)
option(${PROJECT_NAME}_ENABLE_THREAD_SANITIZER
    "Enable thread sanitizer" OFF)


function(enable_sanitizers target_name)

    if(${PROJECT_NAME}_ENABLE_ADDRESS_SANITIZER)
        list(APPEND sanitizer_names "address")
    endif()

    if(${PROJECT_NAME}_ENABLE_MEMORY_SANITIZER)
        list(APPEND sanitizer_names "leak")
    endif()

    if(${PROJECT_NAME}_ENABLE_UNDEFINED_BEHAVIOR_SANITIZER)
        list(APPEND sanitizer_names "undefined")
    endif()

    if(${PROJECT_NAME}_ENABLE_THREAD_SANITIZER)
        list(APPEND sanitizer_names "thread")
    endif()

    if(sanitizer_names)
        list(JOIN sanitizer_names "," sanitizer_names)

        target_compile_options(${target_name}
            INTERFACE -fsanitize=${sanitizer_names})
        target_link_libraries(${target_name}
            INTERFACE -fsanitize=${sanitizer_names})
    endif()

endfunction()
