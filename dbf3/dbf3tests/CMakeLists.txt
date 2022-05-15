cmake_minimum_required(VERSION 3.10)

project(dbf3tests VERSION 0.1 LANGUAGES CXX)
if(NOT CMAKE_BUILD_TYPE)
       set(CMAKE_BUILD_TYPE Release)
endif()
set(CMAKE_SUPPRESS_REGENERATION true)

add_executable(dbf3tests main.cpp)
target_sources(dbf3tests PRIVATE DbfTableTests.cpp)

if(NOT XLIBS_ROOT_FOLDER)
	message(FATAL_ERROR "the XLIBS_ROOT_FOLDER is not set")
	return()
endif()

add_subdirectory(${XLIBS_ROOT_FOLDER}/basic basic_build)

#######################################

find_package(UnitTest++ REQUIRED)
if(NOT UTPP_INCLUDE_DIRS)
	message(FATAL_ERROR "the toolkit UnitTest++ is not found on the system")
	return()
endif()
message(STATUS "UnitTest++ include path: ${UTPP_INCLUDE_DIRS}")

#######################################

if(NOT MSVC OR MSVC_TOOLSET_VERSION LESS_EQUAL 80)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/${CMAKE_BUILD_TYPE})
endif()

set_target_properties(dbf3tests PROPERTIES C_STANDARD 14 C_STANDARD_REQUIRED ON)
target_include_directories(dbf3tests PRIVATE ${XLIBS_ROOT_FOLDER})
target_include_directories(dbf3tests PUBLIC ${UTPP_INCLUDE_DIRS})

target_link_libraries(dbf3tests dbf3)
target_link_libraries(dbf3tests basic UnitTest++)