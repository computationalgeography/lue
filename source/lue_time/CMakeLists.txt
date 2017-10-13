add_test_conditionally(test)

add_library(lue_time INTERFACE
    # src/attribute
)

add_library(lue::lue_time ALIAS lue_time)

### target_include_directories(lue_time SYSTEM
###     PUBLIC
###         ${HDF5_INCLUDE_DIRS}
###     PRIVATE
###         ${Boost_INCLUDE_DIRS}
### )

target_include_directories(lue_time
    INTERFACE
    # PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

### target_link_libraries(lue_time
###     PUBLIC
###         ${HDF5_C_LIBRARIES}
###     PRIVATE
###         Boost::filesystem
### )
### 
### if(HDF5_IS_PARALLEL)
###     target_include_directories(lue_time SYSTEM
###         PUBLIC
###             ${MPI_CXX_INCLUDE_PATH}
###     )
###     target_link_libraries(lue_time
###         PRIVATE
###             MPI::MPI_CXX
###     )
### endif()

# install(
#     TARGETS
#         lue_time
#     EXPORT
#         lue_targets
#     LIBRARY DESTINATION lib
#     ARCHIVE DESTINATION lib
#     RUNTIME DESTINATION bin
# )
# 
# install(
#     DIRECTORY include
#     DESTINATION .
# )
