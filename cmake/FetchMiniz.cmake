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

set(BOOST_ENABLE_CMAKE ON)
set(FETCHCONTENT_UPDATES_DISCONNECTED TRUE)

FetchContent_Declare(
        boost
        GIT_REPOSITORY              https://github.com/boostorg/boost.git
        GIT_TAG                     origin/master
        GIT_SHALLOW                 1
        GIT_PROGRESS                1
        GIT_SUBMODULES              "libs/thread libs/bind libs/config libs/core libs/date_time libs/throw_exception libs/assert libs/smart_ptr libs/winapi libs/predef libs/utility libs/static_assert libs/type_traits libs/numeric libs/mpl libs/preprocessor libs/system libs/chrono libs/ratio libs/integer libs/move libs/io libs/container_hash libs/describe libs/mp11"
        GIT_REMOTE_UPDATE_STRATEGY  CHECKOUT    
)

FetchContent_GetProperties(boost)
if (NOT boost_POPULATED)
    FetchContent_Populate(boost)
endif ()

FetchContent_MakeAvailable(boost)

set(
  Boost_INCLUDE_DIRS
  ${boost_SOURCE_DIR}
  ${boost_BINARY_DIR}
  ${boost_SOURCE_DIR}/libs/bind/include
  ${boost_SOURCE_DIR}/libs/thread/include
  ${boost_SOURCE_DIR}/libs/config/include
  ${boost_SOURCE_DIR}/libs/core/include
  ${boost_SOURCE_DIR}/libs/date_time/include
  ${boost_SOURCE_DIR}/libs/throw_exception/include
  ${boost_SOURCE_DIR}/libs/assert/include
  ${boost_SOURCE_DIR}/libs/smart_ptr/include
  ${boost_SOURCE_DIR}/libs/winapi/include
  ${boost_SOURCE_DIR}/libs/predef/include
  ${boost_SOURCE_DIR}/libs/utility/include
  ${boost_SOURCE_DIR}/libs/static_assert/include
  ${boost_SOURCE_DIR}/libs/type_traits/include
  ${boost_SOURCE_DIR}/libs/numeric/conversion/include
  ${boost_SOURCE_DIR}/libs/mpl/include
  ${boost_SOURCE_DIR}/libs/preprocessor/include
  ${boost_SOURCE_DIR}/libs/system/include
  ${boost_SOURCE_DIR}/libs/chrono/include
  ${boost_SOURCE_DIR}/libs/ratio/include
  ${boost_SOURCE_DIR}/libs/integer/include
  ${boost_SOURCE_DIR}/libs/move/include
  ${boost_SOURCE_DIR}/libs/io/include
  ${boost_SOURCE_DIR}/libs/container_hash/include
  ${boost_SOURCE_DIR}/libs/describe/include
  ${boost_SOURCE_DIR}/libs/mp11/include
)


message("Fetched boost source code from Github: ${boost_SOURCE_DIR}")
include_directories(
  ${Boost_INCLUDE_DIRS}
)

add_library(boost STATIC ${Boost_INCLUDE_DIRS})
target_compile_options(boost INTERFACE -Wno-undef -Wno-language-extension-token -Wno-gnu-anonymous-struct -Wno-old-style-cast -Wno-microsoft-cpp-macro -Wno-sign-conversion -Wno-nested-anon-types -Wno-format-nonliteral -Wno-implicit-int-float-conversion)
set_target_properties(boost PROPERTIES LINKER_LANGUAGE CXX)

set_property(
    TARGET boost PROPERTY
    EXPORT_NAME boost
)

set(FETCHCONTENT_UPDATES_DISCONNECTED FALSE)

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