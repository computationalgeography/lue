file(GLOB_RECURSE pathnames RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} CONFIGURE_DEPENDS "*.py")

foreach(pathname ${pathnames})
    file(GENERATE
        OUTPUT ${CMAKE_BINARY_DIR}/${LUE_BUILD_PYTHON_PACKAGE_DIR}/$<$<BOOL:${LUE_GENERATOR_IS_MULTI_CONFIG}>:$<CONFIG>>/command/${pathname}
        INPUT ${pathname}
    )
endforeach()
