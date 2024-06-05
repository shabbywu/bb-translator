cmake_minimum_required(VERSION 3.13)
set(CMAKE_CXX_STANDARD 20)

set(GLFW_LIBRARY_TYPE "STATIC")
set(VCPKG_OVERLAY_TRIPLETS "${CMAKE_CURRENT_LIST_DIR}/.vcpkg/triplets")
# Enable VCPKG if VCPKG_ROOT defined.
set(vcpkg "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
if(NOT CMAKE_TOOLCHAIN_FILE AND EXISTS "${vcpkg}")
  set(CMAKE_TOOLCHAIN_FILE
      "${vcpkg}"
      CACHE FILEPATH "CMake toolchain file")
  message(STATUS "vcpkg toolchain found: ${CMAKE_TOOLCHAIN_FILE}")
endif()

# Build your app
# ==============
project(bb-translator)
set(CMAKE_BUILD_TYPE "Release")
set(textedit_dir ${CMAKE_CURRENT_LIST_DIR}/externals/ImGuiColorTextEdit)
set(fplus_dir ${CMAKE_CURRENT_LIST_DIR}/externals/FunctionalPlus/include)
set(imgui_markdown_dir ${CMAKE_CURRENT_LIST_DIR}/externals/imgui_markdown)
set(memory_importer_dir
    ${CMAKE_CURRENT_LIST_DIR}/externals/python-memory-importer)
set(py_scripts_dir ${CMAKE_CURRENT_LIST_DIR}/externals/py-scripts)
set(nutcracker_dir ${CMAKE_CURRENT_LIST_DIR}/externals/NutCracker)
set(zip_dir ${CMAKE_CURRENT_LIST_DIR}/externals/zip)

# rename all .bin2app to .cpp
file(GLOB_RECURSE EMBED_CPP "**/*.bin2cpp")
foreach(filename ${EMBED_CPP})
  string(REPLACE ".bin2cpp" ".cpp" filename_cpp ${filename})
  file(COPY_FILE ${filename} ${filename_cpp})
endforeach()

add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
add_subdirectory(src)
