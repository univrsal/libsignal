project(libsignal)
cmake_minimum_required(VERSION 2.8)

if (UNIX AND NOT APPLE)
    add_definitions(-DLINUX=1)
endif()

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -D_DEBUG")

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    add_definitions(-DARCH64=1)
elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
    add_definitions(-DARCH32=1)
endif()

set(LIBS_SOURCE_FILES ./src/signal.cpp ./src/libsignal.h ./src/types.h)
set(TESTS_SOURCE_FILES ./tests/test.cpp ./tests/main.cpp)

add_library("signal" SHARED ${LIBS_SOURCE_FILES})
target_include_directories("signal" PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src/")

# Tests
add_executable("signal_tests" ${TESTS_SOURCE_FILES})
add_dependencies("signal_tests" "signal")
target_include_directories("signal_tests" PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src/")
target_link_libraries("signal_tests" "signal")

# Demos
add_executable("signal_cpp_demo" ./demo/cpp_demo.cpp)
target_include_directories("signal_cpp_demo" PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src/")

add_executable("signal_c_demo" ./demo/c_demo.c)
add_dependencies("signal_c_demo" "signal")
target_include_directories("signal_c_demo" PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src/")
target_link_libraries("signal_c_demo" "signal")

