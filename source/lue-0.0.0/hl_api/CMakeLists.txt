add_test_conditionally(test)

# set(SOURCES
# )
# add_library(lue_hl_api_objects OBJECT
#     ${SOURCES}
# )
# add_library(lue_hl STATIC
#     $<TARGET_OBJECTS:lue_hl_api_objects>
# )
# target_link_libraries(lue_hl
#     lue_cxx
# )
# install(
#     TARGETS lue_hl
#     RUNTIME DESTINATION bin
#     LIBRARY DESTINATION lib
#     ARCHIVE DESTINATION lib
# )
# file(GLOB
#     SOURCES
#     RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
#     *.cpp *.h *.hpp
# )
# install(
#     FILES
#         ${SOURCES}
#     DESTINATION _lue/lue/hl_api
#     COMPONENT python
#     # CMAKE >= 3.6 EXCLUDE_FROM_ALL
# )
