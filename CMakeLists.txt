cmake_minimum_required(VERSION 3.6)
project(g810-led)

set(EXECUTABLE_NAME
        g810-led)

set(BUILD_DIR ${CMAKE_SOURCE_DIR}/bin)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${BUILD_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${BUILD_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BUILD_DIR})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
        src/main.cpp
        src/classes/Keyboard.cpp)

set(HEADER_FILES
        src/classes/Keyboard.h)

add_executable(${EXECUTABLE_NAME} ${SOURCE_FILES} ${HEADER_FILES})

target_link_libraries(${EXECUTABLE_NAME} usb-1.0)