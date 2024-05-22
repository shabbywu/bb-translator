find_package(
  Python3
  COMPONENTS Interpreter Development
  REQUIRED)
find_package(pybind11 CONFIG)

# build memory-importer start
set(SKIP_FIND_PHYSFS 1)
set(BUILD_MEMORY_IMPORTER_MODULE OFF)
set(BUILD_MEMORY_IMPORTER_LIBRARY ON)
add_subdirectory(${memory_importer_dir} memory-importer)
# build memory-importer end

# build nutcracker start
set(SQUIRREL305 TRUE)
add_subdirectory(${nutcracker_dir} nutcracker)
# build nutcracker end

# build zip start
add_subdirectory(${zip_dir} zip)
# build zip end

# pack scripts bundle and move to assets
set(PACK_BB_TRANSLATOR TRUE)
add_subdirectory(${py_scripts_dir}/cmake/pack scripts-bundle)

set(python-cppbundle-dir
    ${CMAKE_CURRENT_BINARY_DIR}/scripts-bundle/bb-translator/bundle)
set(python-zipbundle-dir
    ${CMAKE_CURRENT_BINARY_DIR}/scripts-bundle/bb-translator/build/bundle)

# 嵌入压缩包
file(GLOB source_translator binding.cpp binding.h ${python-cppbundle-dir}/*.cpp
     ${python-cppbundle-dir}/*.h)

add_library(bb_translator_core STATIC ${source_translator})
target_include_directories(
  bb_translator_core PRIVATE ${PHYSFS_INCLUDE_DIR} ${python-cppbundle-dir}/..
                             ${zip_dir}/src/)
target_link_libraries(
  bb_translator_core PRIVATE pybind11::embed Python3::Python zip
                             memory-importer-static nutcrackercpp)

if(MSVC)
  target_compile_options(bb_translator_core PRIVATE /bigobj)
endif(MSVC)
