add_test_conditionally(test)

include(CheckCXXSourceCompiles)
set(CMAKE_REQUIRED_FLAGS -std=c++14)
set(CMAKE_REQUIRED_INCLUDES ${HDF5_INCLUDE_DIRS})
check_cxx_source_compiles("
    #include <hdf5.h>
    #include <type_traits>

    int main() {
        static_assert(std::is_same<hsize_t, uint64_t>::value,
            \"hsize_t not equal to uint64_t\");
        return 0;
    }"
    LUE_HSIZE_T_DEFINED_AS_UINT64_T
)

configure_file(
    configure.h.in
    configure.h
)
set(SOURCES
    attribute
    attributes
    dataset
    dataspace
    datatype
    file
    group
    identifier
    link
)
add_library(lue_hdf5_objects OBJECT
    ${SOURCES}
)
set(lue_object_libraries
    ${lue_object_libraries}
    $<TARGET_OBJECTS:lue_hdf5_objects>
    PARENT_SCOPE
)
# file(GLOB
#     SOURCES
#     RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
#     *.cpp *.h
# )
# install(
#     FILES
#         ${SOURCES}
#     DESTINATION _lue/lue/hdf5
#     COMPONENT python
#     # CMAKE >= 3.6 EXCLUDE_FROM_ALL
# )
