cmake_minimum_required(VERSION 2.8.4)
project(PNGImage)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -g")

set(SOURCE_FILES
    main.cpp
    PNGImage.cpp
    PNGImage.h)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

add_executable(PNGImage ${SOURCE_FILES})
