cmake_minimum_required(VERSION 3.10)
project(basic_test VERSION 0.6.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
endif()

if(MSVC_TOOLSET_VERSION GREATER 80)
	set(Lib_Path1 ${CMAKE_SOURCE_DIR}/../../0libs/Debug)
	set(Lib_Path2 ${CMAKE_SOURCE_DIR}/../../0libs/Release)
else()
	set(Lib_Path1 ${CMAKE_SOURCE_DIR}/../../0libs/${CMAKE_BUILD_TYPE})
	set(Lib_Path2 "")
endif()
set(CMAKE_SUPPRESS_REGENERATION true)

#######################################

find_library(BASIC_LIBRARY NAMES basic PATHS ${Lib_Path1} ${Lib_Path2})
if(NOT BASIC_LIBRARY)
	set(Basic_Lib_Path ${Lib_Path1})
	if(Lib_Path2)
		string(APPEND Basic_Lib_Path " or " ${Lib_Path2})
	endif()

	message(FATAL_ERROR "the library basic is not found at ${Basic_Lib_Path}")
	return()
endif()

add_library(basic UNKNOWN IMPORTED)

if(MSVC_TOOLSET_VERSION GREATER 80)
	string(REPLACE "Debug" "$(Configuration)" BASIC_LIBRARY ${BASIC_LIBRARY})
	string(REPLACE "Release" "$(Configuration)" BASIC_LIBRARY ${BASIC_LIBRARY})
endif()
message(STATUS "basic library path: ${BASIC_LIBRARY}")
set_property(TARGET basic PROPERTY IMPORTED_LOCATION ${BASIC_LIBRARY})

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

add_executable(basic_test main.cpp)
target_include_directories(basic_test PRIVATE ${CMAKE_SOURCE_DIR}/../..)
target_include_directories(basic_test PUBLIC ${UTPP_INCLUDE_DIRS})
target_sources(basic_test PRIVATE DirectoryTests.cpp ExceptionTests.cpp XStringTests.cpp)

target_link_libraries(basic_test basic UnitTest++)