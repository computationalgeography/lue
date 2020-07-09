find_path(GUIDELINE_SUPPORT_LIBRARY_INCLUDE_DIR gsl/gsl)

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(GuidelineSupportLibrary
    DEFAULT_MSG
        GUIDELINE_SUPPORT_LIBRARY_INCLUDE_DIR
)

mark_as_advanced(
    GUIDELINE_SUPPORT_LIBRARY_INCLUDE_DIR
)
