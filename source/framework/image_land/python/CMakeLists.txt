# framework/*
foreach(module_name __init__.py)
    file(GENERATE
        OUTPUT ${CMAKE_BINARY_DIR}/${LUE_BUILD_PYTHON_PACKAGE_DIR}/$<$<BOOL:${LUE_GENERATOR_IS_MULTI_CONFIG}>:$<CONFIG>>/image_land/${module_name}
        INPUT image_land/${module_name}
    )
endforeach()

add_library(lue_py_image_land SHARED)
add_library(lue::py_image_land ALIAS lue_py_image_land)

set_target_properties(lue_py_image_land
    PROPERTIES
        VERSION ${LUE_VERSION}
        SOVERSION ${LUE_VERSION_MAJOR}
)

target_sources(lue_py_image_land
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS
            include
        FILES
            include/lue
    PRIVATE
        source/integrate.cpp
        source/integrate_and_allocate.cpp
        source/python_extension.cpp
        source/submodule.cpp
)

target_include_directories(lue_py_image_land
    PRIVATE
        $<TARGET_PROPERTY:lue::py,SOURCE_DIR>/include  # bind.hpp
)

target_link_libraries(lue_py_image_land
    PRIVATE
        lue::image_land
)

lue_configure_python_shared_library(lue_py_image_land)

lue_install_runtime_libraries(
    TARGETS lue_py_image_land
)

install(
    DIRECTORY
        ${CMAKE_BINARY_DIR}/${LUE_BUILD_PYTHON_PACKAGE_DIR}/$<$<BOOL:${LUE_GENERATOR_IS_MULTI_CONFIG}>:$<CONFIG>>/image_land
    DESTINATION ${LUE_INSTALL_PYTHON_PACKAGE_DIR}
    COMPONENT lue_runtime
)

if(LUE_BUILD_TESTS)
    add_subdirectory(test)
endif()
