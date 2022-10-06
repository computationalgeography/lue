if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    set(CPACK_PACKAGE_NAME lue)
    set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Scientific database and environmental modelling framework")
    set(CPACK_PACKAGE_VENDOR "Computational Geography group, Utrecht University, Netherlands")
    set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
    set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
    set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
    set(CPACK_PACKAGE_INSTALL_DIRECTORY ${CPACK_PACKAGE_NAME})
    set(CPACK_VERBATIM_VARIABLES TRUE)

    # set(CPACK_DESCRIPTION_FILE ${CMAKE_CURRENT_LIST_DIR}/description.txt)
    # set(CPACK_RESOURCE_FILE_WELCOME ${CMAKE_CURRENT_LIST_DIR}/welcome.txt)
    # set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_CURRENT_LIST_DIR}/license.txt)
    # set(CPACK_RESOURCE_FILE_README ${CMAKE_CURRENT_LIST_DIR}/readme.txt)

    set(CPACK_SOURCE_IGNORE_FILES
        /\\.git/
        \\.swp
        # TODO Add more
    )

    # This can be configured conditionally
    # set(CPACK_COMPONENTS_ALL
    #     lue_runtime
    # )

    include(CPack)

    if(WIN32)
        set(CPACK_GENERATOR ZIP)
    else()
        set(CPACK_GENERATOR TGZ)
    endif()

    set(CPACK_ARCHIVE_COMPONENT_INSTALL TRUE)

    set(CPACK_PROJECT_CONFIG_FILE
        ${CMAKE_CURRENT_LIST_DIR}/cpack_generator_overrides.cmake}
    )

    # TODO Factor out targets that depend on GUI stuff into a seperate component. On headless
    #      systems these are not relevant.

    cpack_add_component(lue_runtime
        DISPLAY_NAME "Runtime requirements"
        DESCRIPTION "Shared libraries and executables, for end-users"
        REQUIRED
    )
    cpack_add_component(lue_development
        DISPLAY_NAME "Development requirements"
        DESCRIPTION "Headers and static libraries, for developers"
        DEPENDS lue_runtime
    )



    # Generate source package:

    # All build generators
    # cpack -G TGZ --config CPackSourceConfig.cmake

    # Makefile and Ninja build generators only
    # cmake --build . --target package_source


    # Package multiple configurations:
    # cd <build_dir>
    # cmake --build . config Debug
    # cmake --build . config Release
    # cpack -C "Debug;Release"
endif()
