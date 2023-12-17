cmake_minimum_required(VERSION 3.10)

project(ordersdb_frontend VERSION 1.1 LANGUAGES CXX)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif ()

if (NOT XLIBS_ROOT_FOLDER)
    set(XLIBS_ROOT_FOLDER ${CMAKE_SOURCE_DIR}/..)
endif ()

add_subdirectory(${CMAKE_SOURCE_DIR}/advocats_book adv_build)
add_subdirectory(${CMAKE_SOURCE_DIR}/informers_form informers_build)
add_subdirectory(${CMAKE_SOURCE_DIR}/acts_form acts_build)
add_subdirectory(${CMAKE_SOURCE_DIR}/search_form search_build)