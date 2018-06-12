add_test_conditionally(test)

add_library(lue STATIC

    src/core/array

    # Per object in active set, ID
    # In case of constant collection, the IDs of a single active set
    # are stored. This set does not change through time.
    # src/array/active_id

    # Per idx of location in time, idx of first object in active set
    # In case of asynchronous occurrence, the size of the active set is
    # alway 1, and so, this array is not necessary.
    # src/array/active_set_index


    src/array/same_shape/value


    # src/array/same_shape/constant_shape/value


    # src/array/same_shape/variable_shape/value
    # src/array/same_shape/variable_shape/active_id


    # src/array/different_shape/value


    # src/array/different_shape/constant_shape/value
    # src/array/different_shape/constant_shape/active_object_index  # per object in active set, idx of object array in value_o


    # src/array/different_shape/variable_shape/value


    # Spatio-temporal object information: info
    # - Identity
    # - Locations in time
    # - Locations in space
    # - Property

    # src/info/...



    # Spatio-temporal objects: object
    # - Property set
    # - Phenomenon
    # - Universe

    # src/object/...

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
