
cmake_minimum_required(VERSION 3.16)
project(rocket-sim VERSION 0.1.0 LANGUAGES CXX)

include(FetchContent)

# Eigen
FetchContent_Declare(
  Eigen
  GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
  GIT_TAG 3.4.0
)
set(EIGEN_BUILD_DOC OFF CACHE BOOL "" FORCE)
set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(Eigen)

# GoogleTest
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.14.0
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Main library
add_library(rocket_sim_lib INTERFACE)
target_link_libraries(rocket_sim_lib INTERFACE Eigen3::Eigen)

# Test binary
enable_testing()
add_executable(rocket_sim_tests tests/sim_tests.cpp)
target_link_libraries(rocket_sim_tests
  PRIVATE rocket_sim_lib
  GTest::gtest_main
)
include(GoogleTest)
gtest_discover_tests(rocket_sim_tests)