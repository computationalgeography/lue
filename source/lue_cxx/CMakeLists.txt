add_test_conditionally(test)

add_library(lue STATIC

    src/core/array

    src/array/value_group
    src/array/same_shape/value
    src/array/same_shape/constant_shape/value
    src/array/same_shape/variable_shape/value
    src/array/different_shape/value
    src/array/different_shape/constant_shape/value
    src/array/different_shape/variable_shape/value



    # Spatio-temporal object information: info
    # - Identity
    # - Locations in time
    # - Locations in space
    # - Property

    src/info/identity/active_id
    src/info/identity/active_set_index
    src/info/identity/active_object_index

    # src/info/time/point
    # src/info/time/box
    # src/info/time/grid

    # src/info/space/point
    # src/info/space/box
    # src/info/space/grid

    # src/info/property/...


    # src/object/property_set
    # src/object/phenomenon
    # src/object/universe

)

add_library(lue::lue ALIAS lue)

target_include_directories(lue SYSTEM
    PRIVATE
        ${HDF5_INCLUDE_DIRS}
    PUBLIC
        ${Boost_INCLUDE_DIRS}
)

target_include_directories(lue
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(lue
    PUBLIC
        lue::lue_hdf5
)

install(
    TARGETS
        lue
    EXPORT
        lue_targets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

install(
    DIRECTORY include
    DESTINATION .
)
