file(GLOB sources ${CMAKE_CURRENT_LIST_DIR}/*.h ${CMAKE_CURRENT_LIST_DIR}/*.cpp)
add_library(source_parse STATIC ${sources})
target_include_directories(source_parse PUBLIC ${fplus_dir})
target_link_libraries(source_parse PRIVATE hello-imgui::hello_imgui)

if(IMGUI_MANUAL_BUILD_TESTS)
  add_subdirectory(tests)
endif()
