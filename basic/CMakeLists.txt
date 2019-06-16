cmake_minimum_required(VERSION 3.0)

project(basic VERSION 0.6.1 LANGUAGES CXX)

if(NOT CMAKE_BUILD_TYPE)
       set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../../0libs)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../../0libs)

#add_definitions(-DUSE_BASIC_LIB_AS_DLL -DBASIC_LIB_EXPORTS)

add_library(basic STATIC Exception.cpp PropertiesFile.cpp)
set_target_properties(basic PROPERTIES C_STANDARD 14 C_STANDARD_REQUIRED ON)
