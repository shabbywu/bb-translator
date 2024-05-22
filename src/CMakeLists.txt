set(assets_dir ${CMAKE_CURRENT_LIST_DIR}/assets)
set(temp_assets_dir ${CMAKE_CURRENT_BINARY_DIR}/temp_assets)

find_package(Freetype REQUIRED)
find_package(hello-imgui CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(OpenGL REQUIRED)
find_package(argparse CONFIG REQUIRED)
find_package(frozen CONFIG REQUIRED)

add_subdirectory(ui/imgui_utilities)
add_subdirectory(bb-translator)
add_subdirectory(git)

file(
  GLOB
  sources_bb_translator
  ${CMAKE_CURRENT_LIST_DIR}/*.h
  ${CMAKE_CURRENT_LIST_DIR}/*.cpp
  ${CMAKE_CURRENT_LIST_DIR}/ui/*.h
  ${CMAKE_CURRENT_LIST_DIR}/ui/*.cpp
  ${CMAKE_CURRENT_LIST_DIR}/assets/*/*.cpp)

hello_imgui_add_app(bb_translator ${sources_bb_translator} ASSETS_LOCATION
                    ${temp_assets_dir})

target_link_libraries(
  bb_translator
  PUBLIC glfw
         imgui_utilities
         bb_translator_core
         git_api
         argparse::argparse
         frozen::frozen
         frozen::frozen-headers)
target_include_directories(
  bb_translator
  PUBLIC ${GLFW3_INCLUDE_DIRS} ${textedit_dir} ${fplus_dir}
         ${imgui_markdown_dir} ${GLFW3_LIBRARIES} ${OPENGL_LIBRARIES})

function(_hello_imgui_add_windows_icon app_name ico_location)
  if(NOT EXISTS ${ico_location})
    return()
  endif()

  # Create rc file
  set(icon_rc_file "${CMAKE_CURRENT_BINARY_DIR}/app_${app_name}.rc")
  set(icon_rc_file_contents "IDI_ICON1 ICON DISCARDABLE \"${ico_location}\"")
  file(WRITE ${icon_rc_file} ${icon_rc_file_contents})

  # Add the rc file to the executable
  target_sources(${app_name} PRIVATE ${icon_rc_file})
endfunction()
if(WIN32)
  target_link_options(bb_translator PRIVATE "/SUBSYSTEM:WINDOWS"
                      "/ENTRY:mainCRTStartup")
  _hello_imgui_add_windows_icon(bb_translator ${assets_dir}/favicon.ico)
endif()
