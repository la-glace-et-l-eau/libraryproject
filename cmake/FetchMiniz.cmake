# Fetch miniz source code from Github

include(FetchContent)

SET(FETCHCONTENT_QUIET NO)
SET(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
SET(ENABLE_TESTS OFF)

FetchContent_Declare(
  miniz
  GIT_REPOSITORY              https://github.com/richgel999/miniz
  GIT_TAG                     origin/master
  GIT_SHALLOW                 1
  GIT_PROGRESS                1
  GIT_REMOTE_UPDATE_STRATEGY  CHECKOUT
)

FetchContent_MakeAvailable(miniz)

message("Fetched miniz source code from Github: ${miniz_SOURCE_DIR}")
include_directories(
  ${miniz_SOURCE_DIR}
  ${miniz_BINARY_DIR}
)

    if(NOT CMAKE_BUILD_TYPE)
      set(CMAKE_BUILD_TYPE "Release")
    endif()

    set(CMAKE_MODULE_PATH
            CACHE INTERNAL
            FORCE
            )

    set(FMT_HEADERS
            CACHE INTERNAL
            FORCE
            )

    set(FMT_OS OFF
            CACHE INTERNAL
            "Path to downloaded Catch2 modules"
            FORCE
            )


FetchContent_Declare(
  fmt
  GIT_REPOSITORY              https://github.com/fmtlib/fmt.git
  GIT_TAG                     origin/master
  GIT_SHALLOW                 1
  GIT_PROGRESS                1
  GIT_REMOTE_UPDATE_STRATEGY  CHECKOUT
)

FetchContent_MakeAvailable(fmt)

message("Fetched fmt source code from Github: ${fmt_SOURCE_DIR}")
include_directories(
  ${fmt_SOURCE_DIR}
  ${fmt_BINARY_DIR}
)

# set(MINIZ_SOURCE_URL
#         "https://github.com/richgel999/miniz")
# 
# set(MINIZ_CMAKE_ARGS        "-DCMAKE_PREFIX_PATH=${CMAKE_BINARY_DIR}/thirdparty"
#                             "-DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/thirdparty"
#                             "-DCMAKE_CXX_FLAGS=-D_GLIBCXX_USE_CXX11_ABI=0"
#                             "-DCMAKE_BUILD_TYPE=Release")
# 
# ExternalProject_Add(miniz_ep
#     GIT_REPOSITORY          ${MINIZ_SOURCE_URL}
#     GIT_TAG                 master
#     CMAKE_ARGS              ${MINIZ_CMAKE_ARGS}
# )