cmake_minimum_required(VERSION 3.5)
project (MyTestBeds)

set(CMAKE_CXX_COMPILER clang++)

include_directories(.)


add_subdirectory(test)


aux_source_directory(test TESTSOURCE)

add_executable(test22 ${TESTSOURCE})
