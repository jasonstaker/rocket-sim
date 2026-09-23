cmake_minimum_required(VERSION 3.16)
project(rocket-sim VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Single-config generators (Make, Ninja) default to an empty build type, which
# means no optimization and no debug info. Multi-config generators (Visual
# Studio, Xcode) pick per-build instead, so leave those alone.
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

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
add_library(rocket_sim_lib
  src/vehicle/motor.cpp
  src/dynamics/environment.cpp
)
target_include_directories(rocket_sim_lib PUBLIC include)
target_link_libraries(rocket_sim_lib PUBLIC Eigen3::Eigen)

# Simulation binary
add_executable(3_dof_sim
  src/sim/3_dof_sim.cpp
)
target_link_libraries(3_dof_sim PRIVATE rocket_sim_lib)

# Test binary
enable_testing()
add_executable(rocket_sim_tests
  tests/rk4_tests.cpp
  tests/motor_tests.cpp
  tests/environment_tests.cpp
)
target_link_libraries(rocket_sim_tests
  PRIVATE
  rocket_sim_lib
  GTest::gtest_main
)
include(GoogleTest)
gtest_discover_tests(rocket_sim_tests)
