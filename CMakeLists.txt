cmake_minimum_required(VERSION 3.14)

include(cmake/prelude.cmake)
include(cmake/FetchMiniz.cmake)

SET(CMAKE_C_COMPILER   "/c/Program\ Files/LLVM/bin/clang" CACHE STRING "C++ compiler" FORCE)
SET(CMAKE_CXX_COMPILER "/c/Program\ Files/LLVM/bin/clang++" CACHE STRING "C++ compiler" FORCE)

project(
    libraryproject
    VERSION 0.1.0
    DESCRIPTION "good 2"
    HOMEPAGE_URL "https://example.com/"
    LANGUAGES NONE
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(cmake/project-is-top-level.cmake)
include(cmake/variables.cmake)

# ---- Declare library ----

add_library(libraryproject_libraryproject INTERFACE)
add_library(libraryproject::libraryproject ALIAS libraryproject_libraryproject)

set_property(
    TARGET libraryproject_libraryproject PROPERTY
    EXPORT_NAME libraryproject
)

target_include_directories(
    libraryproject_libraryproject ${warning_guard}
    INTERFACE
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
)

target_compile_features(libraryproject_libraryproject INTERFACE cxx_std_20)

target_link_libraries(libraryproject_libraryproject INTERFACE fmt::fmt)
target_link_libraries(libraryproject_libraryproject INTERFACE miniz)

# ---- Install rules ----

if(NOT CMAKE_SKIP_INSTALL_RULES)
  include(cmake/install-rules.cmake)
endif()

# ---- Developer mode ----

if(NOT libraryproject_DEVELOPER_MODE)
  return()
elseif(NOT PROJECT_IS_TOP_LEVEL)
  message(
      AUTHOR_WARNING
      "Developer mode is intended for developers of libraryproject"
  )
endif()

include(cmake/dev-mode.cmake)
