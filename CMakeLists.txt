cmake_minimum_required (VERSION 3.15)

project(cmake_test
  LANGUAGES CXX
  VERSION 1.0
)

set (CMAKE_CXX_STANDARD 20)

add_subdirectory(src)

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG b796f7d44681514f58a683a3a71ff17c94edb0c1)
FetchContent_MakeAvailable(googletest)

enable_testing()
include(GoogleTest)