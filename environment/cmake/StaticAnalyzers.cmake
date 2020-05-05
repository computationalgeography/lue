option(${PROJECT_NAME}_ENABLE_CPPCHECK "Enable static analysis with cppcheck" OFF)
### option(ENABLE_CLANG_TIDY "Enable static analysis with clang-tidy" OFF)
### option(ENABLE_INCLUDE_WHAT_YOU_USE "Enable static analysis with include-what-you-use" OFF)


if(${PROJECT_NAME}_ENABLE_CPPCHECK)
    find_program(CPPCHECK cppcheck)
    if(CPPCHECK)
        set(CMAKE_CXX_CPPCHECK ${CPPCHECK}
                # Triggered when expression does things other code might
                # depend on. When build without assertions, this is
                # not done. Assume we are not depending on these things.
                --suppress=assertWithSideEffect

                # Triggered when defining a function in a module that
                # is not used within that module. This is common, so
                # get rid of these warnings.
                --suppress=unusedFunction

                --suppress=missingInclude

                # Triggered when a suppressed warning is not actually
                # triggered for a module. So what?
                --suppress=unmatchedSuppression

                # Don't report errors in 3rd-party sources built using
                # FetchContent
                --suppress=*:${CMAKE_BINARY_DIR}/_deps/*

                # We typically create imgui-related objects in the
                # if-conditions which are not used in the blocks, but
                # need to remain in scope
                --suppress=unreadVariable:${CMAKE_SOURCE_DIR}/source/view/src/application.cpp

                # Allow inline suppressions, in comments
                 --inline-suppr

                --enable=all

                # Stop the build in case cppcheck found errors
                --error-exitcode=1

                # --inconclusive -i ${CMAKE_SOURCE_DIR}/imgui/lib
            )
    else()
        message(SEND_ERROR "cppcheck requested but executable not found")
    endif()
endif()


if(${PROJECT_NAME}_ENABLE_CLANG_TIDY)
    find_program(CLANG_TIDY clang-tidy)

    if(CLANGTIDY)
        set(CMAKE_CXX_CLANG_TIDY ${CLANG_TIDY}
                # -extra-arg=-Wno-unknown-warning-option
            )
    else()
        message(SEND_ERROR "clang-tidy requested but executable not found")
    endif()
endif()


### if(ENABLE_INCLUDE_WHAT_YOU_USE)
###   find_program(INCLUDE_WHAT_YOU_USE include-what-you-use)
###   if(INCLUDE_WHAT_YOU_USE)
###     set(CMAKE_CXX_INCLUDE_WHAT_YOU_USE ${INCLUDE_WHAT_YOU_USE})
###   else()
###     message(SEND_ERROR "include-what-you-use requested but executable not found")
###   endif()
### endif()
