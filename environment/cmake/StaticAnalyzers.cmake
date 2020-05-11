option(${PROJECT_NAME}_ENABLE_CPPCHECK
    "Enable static analysis with cppcheck" OFF)
option(${PROJECT_NAME}_ENABLE_CLANG_TIDY
    "Enable static analysis with clang-tidy" OFF)
# option(ENABLE_INCLUDE_WHAT_YOU_USE
#     "Enable static analysis with include-what-you-use" OFF)


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
    find_program(CLANG_TIDY NAMES clang-tidy clang-tidy-10)

    if(CLANG_TIDY)
        set(clang_tidy_checks
                "*"
                "-llvm-header-guard"
                "-modernize-use-trailing-return-type"
                "-fuchsia-default-arguments-calls"
                "-fuchsia-default-arguments-declarations"
                "-cppcoreguidelines-pro-type-vararg"  # BOOST_CHECK(...)
                "-hicpp-vararg"  # BOOST_CHECK(...)
                "-readability-redundant-member-init"

                # TODO We may want to update the sources for these ones
                "-modernize-concat-nested-namespaces"
                "-modernize-pass-by-value"
                "-cppcoreguidelines-pro-bounds-array-to-pointer-decay"
                "-hicpp-no-array-decay"
                "-llvm-include-order"
                "-fuchsia-overloaded-operator"
                "-modernize-use-nodiscard"
                "-fuchsia-statically-constructed-objects"
                "-cert-err58-cpp"
                "-cppcoreguidelines-macro-usage"
                "-cppcoreguidelines-avoid-magic-numbers"
                "-readability-magic-numbers"
                "-google-runtime-references"
                "-modernize-use-default-member-init"
                "-bugprone-macro-parentheses"

            )
        string(REPLACE ";" "," clang_tidy_checks "${clang_tidy_checks}")

        set(CMAKE_CXX_CLANG_TIDY ${CLANG_TIDY}
                --header-filter=.*
                --checks=${clang_tidy_checks}
                --warnings-as-errors=*
            )
    else()
        message(SEND_ERROR "clang-tidy requested but executable not found")
    endif()
endif()


# if(ENABLE_INCLUDE_WHAT_YOU_USE)
#   find_program(INCLUDE_WHAT_YOU_USE include-what-you-use)
#   if(INCLUDE_WHAT_YOU_USE)
#     set(CMAKE_CXX_INCLUDE_WHAT_YOU_USE ${INCLUDE_WHAT_YOU_USE})
#   else()
#     message(SEND_ERROR "include-what-you-use requested but executable not found")
#   endif()
# endif()
