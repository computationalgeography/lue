# This module defines the following CMake variables:
#  GRAPHVIZ_FOUND
#  GRAPHVIZ_DOT_EXECUTABLE

find_program(GRAPHVIZ_DOT_EXECUTABLE dot)


include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Graphviz
    REQUIRED_VARS
        GRAPHVIZ_DOT_EXECUTABLE
)

mark_as_advanced(
    GRAPHVIZ_DOT_EXECUTABLE
)
