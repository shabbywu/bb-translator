find_package(libgit2 CONFIG REQUIRED)

file(GLOB source_git_api *.cpp *.h)
add_library(git_api STATIC ${source_git_api})
target_link_libraries(git_api PRIVATE libgit2::libgit2)
