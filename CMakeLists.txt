cmake_minimum_required(VERSION 3.10)

project(LibAcross VERSION 1.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(WIN32)
    #mingw
    message("Static windows format")
    set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-static")
endif()

add_executable(test src/main-test.cpp)
target_include_directories(test PRIVATE include/)
