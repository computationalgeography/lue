find_program(JupyterBook_EXECUTABLE
    NAMES jupyter-book
)

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(JupyterBook DEFAULT_MSG
    JupyterBook_EXECUTABLE
)

mark_as_advanced(
    JupyterBook_EXECUTABLE
)
