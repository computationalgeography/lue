# set_property(
#     DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
#     PROPERTY RULE_LAUNCH_COMPILE "${CMAKE_COMMAND} -E time")

list(APPEND doxygen_input ${CMAKE_CURRENT_SOURCE_DIR}/doc)

add_subdirectory(core)
list(APPEND doxygen_input ${CMAKE_CURRENT_SOURCE_DIR}/core)

add_subdirectory(partitioned_array)
list(APPEND doxygen_input ${CMAKE_CURRENT_SOURCE_DIR}/partitioned_array)

add_subdirectory(partitioned_route)
list(APPEND doxygen_input ${CMAKE_CURRENT_SOURCE_DIR}/partitioned_route)

add_subdirectory(algorithm)
list(APPEND doxygen_input ${CMAKE_CURRENT_SOURCE_DIR}/algorithm)

add_subdirectory(io)
list(APPEND doxygen_input ${CMAKE_CURRENT_SOURCE_DIR}/io)

if(LUE_FRAMEWORK_WITH_IMAGE_LAND)
    add_subdirectory(image_land)
    list(APPEND doxygen_input ${CMAKE_CURRENT_SOURCE_DIR}/image_land)
endif()

add_subdirectory(model)
list(APPEND doxygen_input ${CMAKE_CURRENT_SOURCE_DIR}/model)

if(LUE_FRAMEWORK_WITH_PYTHON_API)
    add_subdirectory(python)
endif()

if(LUE_BUILD_TESTS)
    add_subdirectory(test)
    list(APPEND doxygen_input ${CMAKE_CURRENT_SOURCE_DIR}/test)
endif()

add_subdirectory(api)

set(doxygen_input ${doxygen_input} PARENT_SCOPE)
