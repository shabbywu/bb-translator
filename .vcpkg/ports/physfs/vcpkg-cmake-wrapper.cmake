find_library(
  PHYSFS_LIBRARY_RELEASE
  NAMES physfs-static NAMES_PER_DIR
  PATHS "${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/lib"
  NO_DEFAULT_PATH)
include(SelectLibraryConfigurations)
select_library_configurations(PHYSFS)
unset(PHYSFS_FOUND)
_find_package(${ARGS})
