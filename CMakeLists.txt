cmake_minimum_required(VERSION 3.5)
project (MyTestBeds)

set(CMAKE_CXX_COMPILER clang++)

include_directories(.)

add_library(base base/timeutils.cc)
add_executable (test_main test/base_test.cc)
target_link_libraries(test_main base)

add_subdirectory(udp_test)