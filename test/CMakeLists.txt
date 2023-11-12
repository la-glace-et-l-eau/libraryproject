cmake_minimum_required(VERSION 3.14)

project(libraryprojectTests LANGUAGES CXX)

include(../cmake/project-is-top-level.cmake)
include(../cmake/folders.cmake)

# ---- Dependencies ----

if(PROJECT_IS_TOP_LEVEL)
  find_package(libraryproject REQUIRED)
  enable_testing()
endif()

# ---- Tests ----

add_executable(libraryproject_test source/libraryproject_test.cpp)
target_link_libraries(libraryproject_test PRIVATE libraryproject::libraryproject)
target_compile_features(libraryproject_test PRIVATE cxx_std_20)

add_test(NAME libraryproject_test COMMAND libraryproject_test)

# ---- End-of-file commands ----

add_folders(Test)
