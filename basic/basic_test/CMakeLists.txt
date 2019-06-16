cmake_minimum_required(VERSION 3.0)

project(basic_test VERSION 0.6.1 LANGUAGES CXX)

if(MSVC_TOOLSET_VERSION GREATER 80)
	set(Basic_Lib_Path1 ${CMAKE_SOURCE_DIR}/../../0libs/Debug)
	set(Basic_Lib_Path2 ${CMAKE_SOURCE_DIR}/../../0libs/Release)
else()
	set(Basic_Lib_Path1 ${CMAKE_SOURCE_DIR}/../../0libs/${CMAKE_BUILD_TYPE})
	set(Basic_Lib_Path2 "")
endif()

find_library(BASIC_LIBRARY NAMES basic PATHS ${Basic_Lib_Path1} ${Basic_Lib_Path2})
if(NOT BASIC_LIBRARY)
	set(Basic_Lib_Path ${Basic_Lib_Path1})
	if(Basic_Lib_Path2)
		string(APPEND Basic_Lib_Path " or " ${Basic_Lib_Path2})
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

find_package(UnitTest++ REQUIRED)
if(NOT UTPP_INCLUDE_DIRS)
	message(FATAL_ERROR "the toolkit UnitTest++ is not found on the system")
	return()
endif()
message(STATUS "UnitTest++ include path: ${UTPP_INCLUDE_DIRS}")

add_executable(basic_test main.cpp ExceptionTests.cpp XStringTests.cpp)
set_target_properties(basic_test PROPERTIES C_STANDARD 14 C_STANDARD_REQUIRED ON)
target_include_directories(basic_test PRIVATE ${CMAKE_SOURCE_DIR}/../..)
target_include_directories(basic_test PUBLIC ${UTPP_INCLUDE_DIRS})

target_link_libraries(basic_test basic UnitTest++)
