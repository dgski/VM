add_subdirectory(assembler)
add_subdirectory(vm)

add_executable(
  shared_test
  shared_test.cpp
)
target_link_libraries(
  shared_test
  GTest::gtest_main
)

enable_testing()
include(GoogleTest)
gtest_discover_tests(shared_test)